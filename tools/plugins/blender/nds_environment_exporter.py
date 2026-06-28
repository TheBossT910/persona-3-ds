import bpy
import struct
import os
import re

from bpy.props import StringProperty, FloatProperty, EnumProperty
from bpy_extras.io_utils import ExportHelper

bl_info = {
    "name": "NDS Studio Exporter & Baker",
    "author": "AI Assistant",
    "version": (4, 0, 1),
    "blender": (4, 0, 0),
    "location": "View3D > Sidebar > NDS Engine",
    "description": "Bakes Vertex Lighting, auto-compresses textures, and maps filepaths for NDS arrays.",
    "category": "Import-Export",
}

# ==============================================================================
# NDS BINARY PACKING UTILITIES
# ==============================================================================

FIFO_COLOR = 0x20
FIFO_TEXCOORD = 0x22
FIFO_BEGIN = 0x40
FIFO_END = 0x41
FIFO_VERTEX16 = 0x23
FIFO_NOP = 0x00
GL_TRIANGLES = 0
GL_QUADS = 1


def sanitize(name: str) -> str:
    return re.sub(r"[^a-zA-Z0-9_]", "_", name)


def nearest_valid_tex_size(size, max_size):
    valid_sizes = [8, 16, 32, 64, 128, 256, 512, 1024]
    best = 8
    for v in valid_sizes:
        if v <= size:
            best = v
        else:
            break
    return min(best, max_size)


def get_material_image(mat):
    if mat and mat.use_nodes:
        for node in mat.node_tree.nodes:
            if node.type == "TEX_IMAGE" and node.image:
                return node.image
    return None


def floattov16(f):
    return max(-32768, min(32767, int(f * (1 << 12)))) & 0xFFFF


def floattot16(f):
    return max(-32768, min(32767, int(f * (1 << 4)))) & 0xFFFF


def to_signed_v16(f):
    return max(-32768, min(32767, int(f * (1 << 12))))


def to_signed_t16(f):
    return int(f * (1 << 4))


def rgb_to_rgb15(color):
    r = int(max(0.0, min(1.0, color[0])) * 31)
    g = int(max(0.0, min(1.0, color[1])) * 31)
    b = int(max(0.0, min(1.0, color[2])) * 31)
    return r | (g << 5) | (b << 10)


def pack_cmds(c1, c2=FIFO_NOP, c3=FIFO_NOP, c4=FIFO_NOP):
    return struct.pack("<I", (c4 << 24) | (c3 << 16) | (c2 << 8) | c1)


# ==============================================================================
# VERTEX LIGHTING BAKER
# ==============================================================================


class OBJECT_OT_nds_bake_vertex_lighting(bpy.types.Operator):
    bl_idname = "object.nds_bake_vertex_lighting"
    bl_label = "Bake Vertex Lighting (NDS Optimized)"
    bl_description = (
        "Bakes Cycles lighting directly into vertex colors to preserve VRAM."
    )

    def execute(self, context):
        scene = context.scene

        valid_meshes = [
            o
            for o in context.selected_objects
            if o.type == "MESH" and len(o.data.polygons) > 0
        ]

        if not valid_meshes:
            self.report({"ERROR"}, "No valid mesh objects with polygons selected.")
            return {"CANCELLED"}

        bpy.ops.object.select_all(action="DESELECT")

        orig_engine = scene.render.engine
        scene.render.engine = "CYCLES"
        scene.cycles.samples = 64
        scene.cycles.bake_type = "DIFFUSE"

        scene.render.bake.use_pass_direct = True
        scene.render.bake.use_pass_indirect = True
        scene.render.bake.use_pass_color = False
        scene.render.bake.target = "VERTEX_COLORS"

        for obj in valid_meshes:
            obj.select_set(True)

            color_attr = obj.data.color_attributes.get("NDS_Illumination")
            if not color_attr:
                color_attr = obj.data.color_attributes.new(
                    name="NDS_Illumination", type="BYTE_COLOR", domain="CORNER"
                )

            obj.data.color_attributes.active_color = color_attr

        context.view_layer.objects.active = valid_meshes[0]

        self.report({"INFO"}, "Baking Vertex Lighting...")

        try:
            bpy.ops.object.bake(type="DIFFUSE")
        except Exception as e:
            scene.render.engine = orig_engine
            self.report({"ERROR"}, f"Bake failed: {str(e)}")
            return {"CANCELLED"}

        clamp = scene.nds_ambient_clamp
        for obj in valid_meshes:
            color_attr = obj.data.color_attributes.get("NDS_Illumination")
            if color_attr:
                for i in range(len(color_attr.data)):
                    c = color_attr.data[i].color
                    color_attr.data[i].color = (
                        max(clamp, c[0]),
                        max(clamp, c[1]),
                        max(clamp, c[2]),
                        c[3],
                    )

        scene.render.engine = orig_engine
        self.report({"INFO"}, "Vertex lighting baked & clamped successfully.")
        return {"FINISHED"}


# ==============================================================================
# BINARY EXPORTER
# ==============================================================================


class EXPORT_SCENE_OT_nds_binary(bpy.types.Operator, ExportHelper):
    bl_idname = "export_scene.nds_binary"
    bl_label = "Export NDS Binary & Header"
    filename_ext = ".bin"
    filter_glob: StringProperty(default="*.bin", options={"HIDDEN"})

    target_scale: FloatProperty(name="Target Scale", default=4.0)

    max_tex_size: EnumProperty(
        name="Max Texture Size",
        description="Strictly clamps and resizes exported PNG layouts to match GRIT targets",
        items=[
            ("8", "8x8", "Limit to 8px maximum"),
            ("16", "16x16", "Limit to 16px maximum"),
            ("32", "32x32", "Limit to 32px maximum"),
            ("64", "64x64", "Limit to 64px maximum"),
            ("128", "128x128", "Limit to 128px maximum"),
            ("256", "256x256", "Limit to 256px maximum"),
            ("512", "512x512", "Limit to 512px maximum"),
        ],
        default="64",
    )

    def execute(self, context):
        mesh_objects = [
            o
            for o in context.selected_objects
            if o.type == "MESH" and len(o.data.polygons) > 0
        ]
        if not mesh_objects:
            self.report({"ERROR"}, "No valid meshes selected for export.")
            return {"CANCELLED"}

        model_name = sanitize(os.path.splitext(os.path.basename(self.filepath))[0])
        export_dir = os.path.dirname(self.filepath)
        bin_out = self.filepath
        h_out = os.path.splitext(bin_out)[0] + ".h"
        max_size_cap = int(self.max_tex_size)
        depsgraph = context.evaluated_depsgraph_get()

        all_verts = []
        for obj in mesh_objects:
            eval_obj = obj.evaluated_get(depsgraph)
            mesh = eval_obj.to_mesh()
            mesh.transform(obj.matrix_world)
            all_verts.extend([v.co for v in mesh.vertices])
            eval_obj.to_mesh_clear()

        xs = [p.x for p in all_verts]
        ys = [p.y for p in all_verts]
        zs = [p.z for p in all_verts]

        max_dim = (
            max(max(xs) - min(xs), max(ys) - min(ys), max(zs) - min(zs))
            if all_verts
            else 1.0
        )
        scale = self.target_scale / max_dim if max_dim > 0 else 1.0
        offset = (
            (min(xs) + max(xs)) / 2.0,
            min(ys),
            (min(zs) + max(zs)) / 2.0,
        )

        mapped_xs = [(x - offset[0]) * scale for x in xs]
        mapped_zs = [-(y - offset[1]) * scale for y in ys]

        min_x, max_x = min(mapped_xs), max(mapped_xs)
        min_z, max_z = min(mapped_zs), max(mapped_zs)

        world_width = max_x - min_x
        world_depth = max_z - min_z
        world_offset_x = (max_x + min_x) / 2.0
        world_offset_z = (max_z + min_z) / 2.0

        tex_registry = {}
        tex_info = []

        for obj in mesh_objects:
            for slot in obj.material_slots:
                if slot.material:
                    img = get_material_image(slot.material)
                    t_name = (
                        sanitize(os.path.splitext(img.name)[0])
                        if img
                        else "default_tex"
                    )
                    if t_name not in tex_registry:
                        w = (
                            nearest_valid_tex_size(img.size[0], max_size_cap)
                            if img
                            else max_size_cap
                        )
                        h = (
                            nearest_valid_tex_size(img.size[1], max_size_cap)
                            if img
                            else max_size_cap
                        )
                        tex_registry[t_name] = len(tex_info)
                        tex_info.append((t_name, w, h, img))

        if not tex_registry:
            tex_registry["default_tex"] = 0
            tex_info.append(("default_tex", max_size_cap, max_size_cap, None))

        for t_name, w, h, img in tex_info:
            if img is not None:
                try:
                    tmp_img = img.copy()
                    tmp_img.scale(w, h)
                    tmp_img.file_format = "PNG"
                    tmp_img.filepath_raw = os.path.join(export_dir, f"{t_name}.png")
                    tmp_img.save()
                    bpy.data.images.remove(tmp_img)
                except Exception as ex:
                    self.report({"WARNING"}, f"Could not process image {t_name}: {ex}")

        sub_lists = {}
        billboards = []

        for tex_name, tex_idx in tex_registry.items():
            words = []
            for obj in mesh_objects:
                is_billboard = obj.name.startswith("BB_")

                eval_obj = obj.evaluated_get(depsgraph)
                mesh = eval_obj.to_mesh()
                mesh.transform(obj.matrix_world)
                mesh.calc_loop_triangles()

                uv_layer = mesh.uv_layers.active

                # THE FIX: STRICTLY ONLY USE OUR BAKED LIGHTING LAYER
                # This safely ignores imported garbage/ripped vertex colors.
                color_layer = mesh.color_attributes.get("NDS_Illumination")

                faces_for_tex = []
                for tri in mesh.loop_triangles:
                    mat = (
                        obj.material_slots[tri.material_index].material
                        if (
                            obj.material_slots
                            and tri.material_index < len(obj.material_slots)
                        )
                        else None
                    )
                    m_img = get_material_image(mat)
                    m_tname = (
                        sanitize(os.path.splitext(m_img.name)[0])
                        if m_img
                        else "default_tex"
                    )
                    if m_tname == tex_name:
                        faces_for_tex.append(tri)

                if faces_for_tex:
                    if is_billboard:
                        b_verts = [
                            mesh.vertices[loop].co
                            for tri in faces_for_tex
                            for loop in tri.loops
                        ]
                        bxs = [v.x for v in b_verts]
                        bys = [v.y for v in b_verts]
                        bzs = [v.z for v in b_verts]
                        cx = ((min(bxs) + max(bxs)) / 2.0 - offset[0]) * scale
                        cy = ((min(bzs) + max(bzs)) / 2.0 - offset[2]) * scale
                        cz = -((min(bys) + max(bys)) / 2.0 - offset[1]) * scale
                        hw = (max(bxs) - min(bxs)) / 2.0 * scale
                        hh = (max(bzs) - min(bzs)) / 2.0 * scale

                        u_vals, v_vals = [], []
                        for tri in faces_for_tex:
                            for loop_idx in tri.loops:
                                if uv_layer:
                                    uv = uv_layer.data[loop_idx].uv
                                    u_vals.append(uv.x)
                                    v_vals.append(uv.y)
                        if u_vals:
                            u_min, u_max = min(u_vals), max(u_vals)
                            v_min_raw, v_max_raw = min(v_vals), max(v_vals)
                            v_min = 1.0 - v_max_raw
                            v_max = 1.0 - v_min_raw
                        else:
                            u_min, u_max, v_min, v_max = 0.0, 1.0, 0.0, 1.0

                        tw, th = tex_info[tex_idx][1], tex_info[tex_idx][2]
                        billboards.append(
                            {
                                "cx": cx,
                                "cy": cy,
                                "cz": cz,
                                "hw": hw,
                                "hh": hh,
                                "tex_slot": tex_idx,
                                "u0_16": to_signed_t16(u_min * tw),
                                "v0_16": to_signed_t16(v_min * th),
                                "u1_16": to_signed_t16(u_max * tw),
                                "v1_16": to_signed_t16(v_max * th),
                            }
                        )
                    else:
                        words.append(pack_cmds(FIFO_BEGIN))
                        words.append(struct.pack("<I", GL_TRIANGLES))

                        for tri in faces_for_tex:
                            for loop_idx in tri.loops:
                                loop = mesh.loops[loop_idx]

                                col = [1.0, 1.0, 1.0]
                                if color_layer:
                                    col = color_layer.data[loop_idx].color
                                words.append(pack_cmds(FIFO_COLOR))
                                words.append(struct.pack("<I", rgb_to_rgb15(col)))

                                uv_x, uv_y = 0.0, 0.0
                                if uv_layer:
                                    uv_x = uv_layer.data[loop_idx].uv.x
                                    uv_y = uv_layer.data[loop_idx].uv.y

                                tw, th = tex_info[tex_idx][1], tex_info[tex_idx][2]
                                u16 = floattot16(uv_x * tw)
                                v16 = floattot16((1.0 - uv_y) * th)
                                words.append(pack_cmds(FIFO_TEXCOORD))
                                words.append(
                                    struct.pack(
                                        "<I", (u16 & 0xFFFF) | ((v16 & 0xFFFF) << 16)
                                    )
                                )

                                vx, vy, vz = mesh.vertices[loop.vertex_index].co
                                sx = (vx - offset[0]) * scale
                                sy = (vz - offset[2]) * scale
                                sz = -(vy - offset[1]) * scale

                                words.append(pack_cmds(FIFO_VERTEX16))
                                words.append(
                                    struct.pack(
                                        "<I", (floattov16(sy) << 16) | floattov16(sx)
                                    )
                                )
                                words.append(struct.pack("<I", floattov16(sz)))

                        words.append(pack_cmds(FIFO_END))
                        words.append(struct.pack("<I", 0))

                eval_obj.to_mesh_clear()
            sub_lists[tex_idx] = words

        billboards.sort(key=lambda b: b["tex_slot"])

        old_to_new = {}
        tex_info_geo = []
        for old_idx, entry in enumerate(tex_info):
            words = sub_lists.get(old_idx, [])
            if words:
                old_to_new[old_idx] = len(tex_info_geo)
                tex_info_geo.append((entry, words))
            else:
                old_to_new[old_idx] = None

        for b in billboards:
            new_slot = old_to_new.get(b["tex_slot"])
            b["tex_slot_remapped"] = new_slot if new_slot is not None else b["tex_slot"]

        with open(bin_out, "wb") as f:
            f.write(b"ENV1")
            f.write(struct.pack("<I", len(tex_info_geo)))
            for _, words in tex_info_geo:
                f.write(struct.pack("<I", len(words)))
                for w in words:
                    f.write(w)

        n_geo = len(tex_info_geo)
        n_tex = len(tex_info)
        n_geo
        max(n_tex, 1)
        UP = model_name.upper()

        with open(h_out, "w") as h:
            h.write("#pragma once\n// Auto-generated by NDS Studio Exporter\n")
            h.write(
                f"// Source: {model_name}\n// Scale: {scale:.6f}\n// DO NOT EDIT - regenerate from source.\n\n"
            )
            h.write(
                "#include <math.h>\n#include <nds.h>\n#include <stdio.h>\n#include <stdlib.h>\n\n"
            )

            h.write("// World bounds\n")
            h.write(f"#define {UP}_WORLD_OFFSET_X {world_offset_x:.6f}f\n")
            h.write(f"#define {UP}_WORLD_OFFSET_Z {world_offset_z:.6f}f\n")
            h.write(f"#define {UP}_WORLD_WIDTH    {world_width:.6f}f\n")
            h.write(f"#define {UP}_WORLD_DEPTH    {world_depth:.6f}f\n\n")

            h.write(
                f"static const char* const {model_name}_TextureFilenames[{n_tex}] = {{\n"
            )
            for t_name, _, _, _ in tex_info:
                h.write(f'    "{t_name}",\n')
            h.write("};\n\n")

            h.write(f"enum {model_name}_TexSlot\n{{\n")
            for i, (t_name, _, _, _) in enumerate(tex_info):
                h.write(f"    {UP}_TEX_{t_name.upper()} = {i},\n")
            h.write(f"    {UP}_TEX_COUNT = {n_tex}\n}};\n\n")

            h.write(f"struct {model_name}_BillboardData\n{{\n")
            h.write("    v16 x, y, z;\n")
            h.write("    v16 halfWidth, halfHeight;\n")
            h.write("    int texSlot;\n")
            h.write("    short u0, v0, u1, v1;\n")
            h.write("};\n\n")

            dl_safe = max(n_geo, 1)
            tex_safe = max(n_tex, 1)
            h.write(f"class {model_name}_Environment\n{{\n  public:\n")
            h.write(f"    u32* displayLists[{dl_safe}];\n")
            h.write(f"    u32  dlSizes[{dl_safe}];\n")
            h.write(f"    int  textureIDs[{tex_safe}];\n\n")
            h.write(f"    static const int BILLBOARD_COUNT = {len(billboards)};\n")

            if billboards:
                h.write(
                    f"    const {model_name}_BillboardData BILLBOARDS[{len(billboards)}] = {{\n"
                )
                for b in billboards:
                    h.write(
                        f"        {{ {to_signed_v16(b['cx'])}, {to_signed_v16(b['cy'])}, "
                        f"{to_signed_v16(b['cz'])}, "
                        f"{to_signed_v16(b['hw'])}, {to_signed_v16(b['hh'])}, "
                        f"{b['tex_slot_remapped']}, "
                        f"{b['u0_16']}, {b['v0_16']}, {b['u1_16']}, {b['v1_16']} }},\n"
                    )
                h.write("    };\n\n")
            else:
                h.write(
                    f"    const {model_name}_BillboardData* BILLBOARDS = nullptr;\n\n"
                )

            h.write(f"    {model_name}_Environment()\n    {{\n")
            h.write(f"        for (int i = 0; i < {dl_safe}; i++)\n        {{\n")
            h.write("            displayLists[i] = nullptr; dlSizes[i] = 0;\n")
            h.write("        }\n")
            h.write(f"        for (int i = 0; i < {tex_safe}; i++)\n        {{\n")
            h.write("            textureIDs[i] = 0;\n")
            h.write("        }\n    }\n\n")

            h.write(
                f"    bool load(const char* filepath, const unsigned int* bitmaps[{tex_safe}])\n    {{\n"
            )
            h.write('        FILE* file = fopen(filepath, "rb");\n')
            h.write("        if (!file) return false;\n\n")
            h.write("        char magic[4];\n        fread(magic, 1, 4, file);\n")
            h.write(
                "        if (magic[0]!='E'||magic[1]!='N'||magic[2]!='V'||magic[3]!='1') { fclose(file); return false; }\n\n"
            )
            h.write(
                "        u32 groupCount;\n        fread(&groupCount, sizeof(u32), 1, file);\n"
            )
            h.write(
                f"        if (groupCount != {n_geo}) {{ fclose(file); return false; }}\n\n"
            )
            h.write("        for (u32 i = 0; i < groupCount; i++)\n        {\n")
            h.write("            fread(&dlSizes[i], sizeof(u32), 1, file);\n")
            h.write(
                "            displayLists[i] = (u32*)malloc((dlSizes[i] + 1) * sizeof(u32));\n"
            )
            h.write("            displayLists[i][0] = dlSizes[i];\n")
            h.write(
                "            if (dlSizes[i] > 0) fread(&displayLists[i][1], sizeof(u32), dlSizes[i], file);\n"
            )
            h.write("        }\n        fclose(file);\n\n")

            for i, (t_name, w, h_px, _) in enumerate(tex_info):
                h.write(f"        if (bitmaps[{i}])\n        {{\n")
                h.write(f"            glGenTextures(1, &textureIDs[{i}]);\n")
                h.write(f"            glBindTexture(GL_TEXTURE_2D, textureIDs[{i}]);\n")
                h.write(
                    f"            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_SIZE_{w}, TEXTURE_SIZE_{h_px}, 0,"
                    f" TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T, bitmaps[{i}]);\n"
                )
                h.write("        }\n")
            h.write("        return true;\n    }\n\n")

            h.write("    void draw()\n    {\n")
            for i, ((t_name, w, h_px, _), _) in enumerate(tex_info_geo):
                orig_idx = next(
                    oi for oi, entry in enumerate(tex_info) if entry[0] == t_name
                )
                if i > 0:
                    h.write("        while (GFX_BUSY);\n")
                h.write(
                    f"        glBindTexture(GL_TEXTURE_2D, textureIDs[{orig_idx}]);\n"
                )
                h.write(
                    f"        if (displayLists[{i}]) glCallList(displayLists[{i}]);\n"
                )
            h.write("        while (GFX_BUSY);\n")
            h.write("    }\n\n")

            h.write(
                "    void drawBillboards(bool faceCamera, float camX, float camY, float camZ)\n    {\n"
            )
            h.write("        if (BILLBOARD_COUNT == 0) return;\n")
            h.write("        int  currentSlot = -1;\n")
            h.write("        bool inQuads     = false;\n\n")
            h.write("        for (int i = 0; i < BILLBOARD_COUNT; i++)\n        {\n")
            h.write(
                f"            const {model_name}_BillboardData& bb = BILLBOARDS[i];\n"
            )
            h.write("            if (bb.texSlot != currentSlot)\n            {\n")
            h.write("                if (inQuads) { glEnd(); inQuads = false; }\n")
            h.write("                while (GFX_BUSY);\n")
            h.write(
                "                glBindTexture(GL_TEXTURE_2D, textureIDs[bb.texSlot]);\n"
            )
            h.write("                currentSlot = bb.texSlot;\n")
            h.write("            }\n")
            h.write(
                "            if (!inQuads) { glBegin(GL_QUADS); inQuads = true; }\n\n"
            )
            h.write("            v16 rX = (v16)(4096), rY = 0, rZ = 0;\n")
            h.write("            v16 uX = 0, uY = (v16)(4096), uZ = 0;\n\n")
            h.write("            if (faceCamera)\n            {\n")
            h.write("                float bx = (float)bb.x / 4096.0f;\n")
            h.write("                float bz = (float)bb.z / 4096.0f;\n")
            h.write("                float dx = camX - bx, dz = camZ - bz;\n")
            h.write("                float dist = sqrtf(dx*dx + dz*dz);\n")
            h.write("                if (dist > 0.001f) { dx /= dist; dz /= dist; }\n")
            h.write("                rX = (v16)(dz * 4096.0f);\n")
            h.write("                rZ = (v16)(-dx * 4096.0f);\n")
            h.write("            }\n\n")
            h.write(
                "            v16 rx = mulf32(rX, bb.halfWidth),  ry = mulf32(rY, bb.halfWidth),  rz = mulf32(rZ, bb.halfWidth);\n"
            )
            h.write(
                "            v16 ux = mulf32(uX, bb.halfHeight), uy = mulf32(uY, bb.halfHeight), uz = mulf32(uZ, bb.halfHeight);\n\n"
            )
            h.write(
                "            glTexCoord2t16(bb.u0, bb.v1); glVertex3v16(bb.x-rx-ux, bb.y-ry-uy, bb.z-rz-uz);\n"
            )
            h.write(
                "            glTexCoord2t16(bb.u1, bb.v1); glVertex3v16(bb.x+rx-ux, bb.y+ry-uy, bb.z+rz-uz);\n"
            )
            h.write(
                "            glTexCoord2t16(bb.u1, bb.v0); glVertex3v16(bb.x+rx+ux, bb.y+ry+uy, bb.z+rz+uz);\n"
            )
            h.write(
                "            glTexCoord2t16(bb.u0, bb.v0); glVertex3v16(bb.x-rx+ux, bb.y-ry+uy, bb.z-rz+uz);\n"
            )
            h.write("        }\n")
            h.write("        if (inQuads) { glEnd(); }\n")
            h.write("        while (GFX_BUSY);\n")
            h.write("    }\n\n")

            h.write("    void cleanup()\n    {\n")
            h.write(f"        for (u32 i = 0; i < {dl_safe}; i++)\n        {{\n")
            h.write(
                "            if (displayLists[i]) { free(displayLists[i]); displayLists[i] = nullptr; }\n"
            )
            h.write("        }\n")
            h.write(f"        glDeleteTextures({tex_safe}, textureIDs);\n")
            h.write("    }\n};\n")

        self.report(
            {"INFO"},
            f"Exported {model_name}.bin + .h  ({n_geo} DL group(s), {len(billboards)} billboard(s)).",
        )
        return {"FINISHED"}


class VIEW3D_PT_nds_dev_panel(bpy.types.Panel):
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "NDS Engine"
    bl_label = "Production Map Utility"

    def draw(self, context):
        layout = self.layout
        scene = context.scene

        col = layout.column(align=True)
        col.prop(scene, "nds_ambient_clamp", text="Ambient Brightness")
        col.operator("object.nds_bake_vertex_lighting", icon="LIGHT_SUN")
        layout.separator()
        col.operator(
            "export_scene.nds_binary",
            icon="EXPORT",
            text="Compile Direct to Binary (.bin)",
        )


def menu_func_export(self, context):
    self.layout.operator(
        EXPORT_SCENE_OT_nds_binary.bl_idname, text="NDS Engine Binary (.bin)"
    )


def register():
    bpy.types.Scene.nds_ambient_clamp = FloatProperty(
        name="Ambient Clamp",
        description="Prevents shadows from hitting absolute zero, ensuring DS textures remain visible.",
        default=0.25,
        min=0.0,
        max=1.0,
    )
    bpy.utils.register_class(OBJECT_OT_nds_bake_vertex_lighting)
    bpy.utils.register_class(EXPORT_SCENE_OT_nds_binary)
    bpy.utils.register_class(VIEW3D_PT_nds_dev_panel)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    del bpy.types.Scene.nds_ambient_clamp
    bpy.utils.unregister_class(OBJECT_OT_nds_bake_vertex_lighting)
    bpy.utils.unregister_class(EXPORT_SCENE_OT_nds_binary)
    bpy.utils.unregister_class(VIEW3D_PT_nds_dev_panel)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()
