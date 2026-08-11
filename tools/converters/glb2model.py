import sys
import re
import struct

import numpy as np
import pygltflib

MAGIC = b"MDL2"
MAX_16_BIT_INT = 32767

# NDS GPU Commands
FIFO_BEGIN = 0x40
FIFO_TEXCOORD = 0x22
FIFO_VERTEX = 0x24
FIFO_END = 0x41
GL_TRIANGLES = 0

# Component type mappings from GLTF to numpy dtypes
DTYPE_MAP = {
    5120: np.int8,
    5121: np.uint8,
    5122: np.int16,
    5123: np.uint16,
    5125: np.uint32,
    5126: np.float32,
}
TYPE_COUNTS = {"SCALAR": 1, "VEC2": 2, "VEC3": 3, "VEC4": 4, "MAT4": 16}


def float_to_v16(val):
    """Converts a float to a Nintendo DS 4.12 fixed-point integer."""
    i = int(round(val * 4096.0))
    return max(-MAX_16_BIT_INT, min(MAX_16_BIT_INT, i))


def pack_uv_t16(u, v, tex_w, tex_h):
    """Packs UV into DS TEXTURE_PACK t16 format."""
    u_p = min(tex_w - 1, max(0, int(u * (tex_w - 1))))
    v_p = min(tex_h - 1, max(0, int((1.0 - v) * (tex_h - 1))))
    u_t16 = (u_p << 4) & 0xFFFF  # TODO: split into float_to_t16 func
    v_t16 = (v_p << 4) & 0xFFFF
    return u_t16 | (v_t16 << 16)


def read_accessor_data(gltf, accessor_index):
    if accessor_index is None:
        return None
    accessor = gltf.accessors[accessor_index]
    buffer_view = gltf.bufferViews[accessor.bufferView]

    # There are other ways to get the data, but they don't always work (seems to be based on the pygltflib version). This method worked in this scenario so we're going with that.
    data = (
        gltf.binary_blob()
        if callable(getattr(gltf, "binary_blob", None))
        else getattr(gltf, "binary_data", None)
    )
    if data is None:
        raise ValueError("Could not extract binary buffer data from GLB File")

    byte_offset = (buffer_view.byteOffset or 0) + (accessor.byteOffset or 0)
    dtype = DTYPE_MAP[accessor.componentType]
    count = accessor.count
    num_components = TYPE_COUNTS[accessor.type]

    raw_array = np.frombuffer(
        data[
            byte_offset : byte_offset
            + count * num_components * np.dtype(dtype).itemsize
        ],
        dtype=dtype,
    )
    if num_components > 1:
        return raw_array.reshape(count, num_components)
    return raw_array


def build_nds_display_list(positions, uvs, indices, tex_w, tex_h):
    """Compiles GL_TRIANGLES primitive data into NDS display list format."""
    dl_words = []

    flat_indices = (
        indices.flatten() if indices is not None else np.arange(len(positions))
    )

    # Cmmand: BEGIN & GL_TRIANGLES
    dl_words.append(FIFO_BEGIN)
    dl_words.append(GL_TRIANGLES)

    for i in range(0, len(flat_indices), 3):
        for j in range(3):
            v_idx = flat_indices[i + j]

            # Pack UV Command
            if uvs is not None:
                u, v = uvs[v_idx]
                dl_words.append(FIFO_TEXCOORD)
                dl_words.append(pack_uv_t16(u, v, tex_w, tex_h))

            # Pack Vertex Command
            vx, vy, vz = positions[v_idx]
            v16_x = (
                float_to_v16(vx) & 0xFFFF
            )  # TODO: move 0xFFFF in conversion function?
            v16_y = float_to_v16(vy) & 0xFFFF
            v16_z = float_to_v16(vz) & 0xFFFF

            dl_words.append(FIFO_VERTEX)
            dl_words.append(v16_x | (v16_y << 16))
            dl_words.append(v16_z)

    # Command: END
    dl_words.append(FIFO_END)
    return dl_words


# ---------------------------------------------------------------------------
# MDL2 binary format
# ---------------------------------------------------------------------------
#
# Header      : 'MDL2' | u32 nodeCount | u32 animCount | u32 texCount
# Tex table   : texCount × { char[64] name | u16 w | u16 h | u8 isRGBA | u8[3] pad }
# Nodes       : nodeCount × { s32 pid | s32 px | s32 py | s32 pz |
#                              u32 subListCount |
#                              subListCount × { s32 texSlot | u32 dlSize | u32[dlSize] } }
# Animations  : animCount × { char[32] name | u32 duration |
#                              nodeCount × { u32 kfCount | Keyframe[kfCount] } }
#
# Keyframe (16 bytes)  : s32 time | s16 rx | s16 ry | s16 rz | s16 px | s16 py | s16 pz
#
# texSlot == -1 means "no texture bound for this sub-list".


# TODO: Try to remove?
def get_prop(obj, key, default=None):
    """Helper to read attributes safely from either pygltflib objects or dicts."""
    if obj is None:
        return default
    if isinstance(obj, dict):  # Dictionary Check
        return obj.get(key, default)
    return getattr(obj, key, default)  # Object Check


# TODO: Add support for animations
# TODO: Refactor into subfunctions for readability
def convert_glb_to_mdl2(glb_path, output_path):
    gltf = pygltflib.GLTF2().load(glb_path)

    # Textures table
    textures = []
    for image in gltf.images:
        tex_name = get_prop(image, "name", "texture")
        clean_name = re.sub(r"[^a-zA-Z0-9_]", "_", tex_name)[:63]
        textures.append(
            {"name": clean_name.ljust(64, "\0"), "w": 128, "h": 128, "isRGBA": 0}
        )
    tex_count = len(textures)

    # Base Mesh Hierarchy Nodes
    nodes = []
    for index, node in enumerate(gltf.nodes):
        # Find parent node index
        pid = -1
        for parent_index, parent_node in enumerate(gltf.nodes):
            children = get_prop(parent_node, "children", [])
            if children and index in children:
                pid = parent_index
                break

        translation = get_prop(node, "translation", [0.0, 0.0, 0.0])
        px, py, pz = (
            [float_to_v16(v) for v in translation] if translation else (0, 0, 0)
        )

        mesh_id = get_prop(node, "mesh", None)
        sub_lists = []

        if mesh_id is not None and mesh_id < len(gltf.meshes):
            mesh = gltf.meshes[mesh_id]
            primitives = get_prop(mesh, "primitives", [])

            for prim in primitives:
                attrs = get_prop(prim, "attributes", {})
                pos_index = get_prop(attrs, "POSITION")
                uv_index = get_prop(attrs, "TEXCOORD_0")
                indices_index = get_prop(prim, "indices")
                mat_index = get_prop(prim, "material")

                positions = read_accessor_data(gltf, pos_index)
                indices = read_accessor_data(gltf, indices_index)
                uvs = read_accessor_data(gltf, uv_index)

                tex_slot = (
                    mat_index if mat_index is not None and mat_index < tex_count else -1
                )
                tex_w = textures[tex_slot]["w"] if tex_slot >= 0 else 128
                tex_h = textures[tex_slot]["h"] if tex_slot >= 0 else 128

                dl_words = build_nds_display_list(positions, uvs, indices, tex_w, tex_h)

                sub_lists.append(
                    {"texSlot": tex_slot, "dlSize": len(dl_words), "dlWords": dl_words}
                )

        nodes.append(
            {
                "pid": pid,
                "px": px,
                "py": py,
                "pz": pz,
                "subListCount": len(sub_lists),
                "subLists": sub_lists,
            }
        )

    # Write to bin file
    with open(output_path, "wb") as f:
        # Header: 'MDL2 | u32 nodeCount | u32 animCount | u32 texCount
        f.write(struct.pack("<4sIII", MAGIC, len(nodes), 0, tex_count))

        # Tex Table
        for tex in textures:
            f.write(
                struct.pack(
                    "<64sHHB3s",
                    tex["name"].encode("ascii"),
                    tex["w"],
                    tex["h"],
                    tex["isRGBA"],
                    b"\0\0\0",
                )
            )

        # Nodes
        for node in nodes:
            f.write(
                struct.pack(
                    "<iiiiI",
                    node["pid"],
                    node["px"],
                    node["py"],
                    node["pz"],
                    node["subListCount"],
                )
            )
            for sub_list in node["subLists"]:
                f.write(struct.pack("<iI", sub_list["texSlot"], sub_list["dlSize"]))
                for word in sub_list["dlWords"]:
                    f.write(struct.pack("<I", word))

        print(f"Outputted '{output_path}' ({len(nodes)} nodes, {tex_count} textures)")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python glb2model.py <input.glb> <output.mdl2>")
        sys.exit(1)

    input_glb = sys.argv[1]
    output_mdl2 = sys.argv[2]

    convert_glb_to_mdl2(input_glb, output_mdl2)
