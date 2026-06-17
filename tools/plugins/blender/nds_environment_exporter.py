import bpy
import struct
import os
import re

from bpy.props import StringProperty, FloatProperty, EnumProperty
from bpy_extras.io_utils import ExportHelper

bl_info = {
    "name": "NDS Studio Exporter & Baker",
    "author": "AI Assistant",
    "version": (3, 4, 0),
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

        scene.render.engine = orig_engine
        self.report({"INFO"}, "Vertex lighting baked successfully. VRAM preserved.")
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

        tex_registry = {}
        tex_info = []

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
        offset = ((min(xs) + max(xs)) / 2.0, min(ys), (min(zs) + max(zs)) / 2.0)

        mapped_xs, mapped_zs = [], []
        for x, y, z in zip(xs, ys, zs):
            mapped_xs.append((x - offset[0]) * scale)
            mapped_zs.append(-(y - offset[1]) * scale)

        min_x = min(mapped_xs) if mapped_xs else 0
        max_x = max(mapped_xs) if mapped_xs else 0
        min_z = min(mapped_zs) if mapped_zs else 0
        max_z = max(mapped_zs) if mapped_zs else 0

        world_width = max_x - min_x
        world_depth = max_z - min_z
        world_offset_x = (max_x + min_x) / 2.0
        world_offset_z = (max_z + min_z) / 2.0

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

                    target_png_path = os.path.join(export_dir, f"{t_name}.png")
                    tmp_img.filepath_raw = target_png_path
                    tmp_img.save()

                    bpy.data.images.remove(tmp_img)
                except Exception as ex:
                    self.report({"WARNING"}, f"Could not process image {t_name}: {ex}")

        sub_lists = {}
        for tex_name, tex_idx in tex_registry.items():
            words = []
            for obj in mesh_objects:
                eval_obj = obj.evaluated_get(depsgraph)
                mesh = eval_obj.to_mesh()
                mesh.transform(obj.matrix_world)
                mesh.calc_loop_triangles()

                uv_layer = mesh.uv_layers.active
                color_layer = (
                    mesh.color_attributes.active_color
                    if mesh.color_attributes
                    else None
                )

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

                eval_obj.to_mesh_clear()
            sub_lists[tex_idx] = words

        with open(bin_out, "wb") as f:
            f.write(b"ENV1")
            f.write(struct.pack("<I", len(tex_info)))

            for i in range(len(tex_info)):
                words = sub_lists.get(i, [])
                f.write(struct.pack("<I", len(words)))
                for w in words:
                    f.write(w)

        with open(h_out, "w") as h:
            h.write("#pragma once\n")
            h.write("// Auto-generated by NDS Studio Exporter\n")
            h.write(f"// Source: {model_name}\n")
            h.write(f"// Scale: {scale:.6f}\n")
            h.write("// DO NOT EDIT - regenerate from source.\n\n")

            h.write("#include <math.h>\n")
            h.write("#include <nds.h>\n")
            h.write("#include <stdio.h>\n")
            h.write("#include <stdlib.h>\n\n")

            h.write("// World bounds\n")
            h.write(
                f"#define {model_name.upper()}_WORLD_OFFSET_X {world_offset_x:.6f}f\n"
            )
            h.write(
                f"#define {model_name.upper()}_WORLD_OFFSET_Z {world_offset_z:.6f}f\n"
            )
            h.write(f"#define {model_name.upper()}_WORLD_WIDTH {world_width:.6f}f\n")
            h.write(f"#define {model_name.upper()}_WORLD_DEPTH {world_depth:.6f}f\n\n")

            # --- NEW: AUTO-GENERATED FILENAMES ARRAY ---
            h.write(
                f"static const char* const {model_name}_TextureFilenames[{len(tex_info)}] = {{\n"
            )
            for _, (t_name, _, _, _) in enumerate(tex_info):
                h.write(f'    "{t_name}",\n')
            h.write("};\n\n")

            h.write(f"enum {model_name}_TexSlot\n{{\n")
            for i, (t_name, _, _, _) in enumerate(tex_info):
                h.write(f"    {model_name.upper()}_TEX_{t_name.upper()} = {i},\n")
            h.write(f"    {model_name.upper()}_TEX_COUNT = {len(tex_info)}\n}};\n\n")

            h.write(f"struct {model_name}_BillboardData\n{{\n")
            h.write(
                "    v16 x, y, z;\n    v16 halfWidth, halfHeight;\n    int texSlot;\n    short u0, v0, u1, v1;\n"
            )
            h.write("};\n\n")

            h.write(f"class {model_name}_Environment\n{{\n  public:\n")
            h.write(f"    u32* displayLists[{len(tex_info)}];\n")
            h.write(f"    u32 dlSizes[{len(tex_info)}];\n")
            h.write(f"    int textureIDs[{len(tex_info)}];\n\n")

            h.write("    static const int BILLBOARD_COUNT = 0;\n")
            h.write(
                f"    const {model_name}_BillboardData BILLBOARDS[1] = {{ 0 }};\n\n"
            )

            h.write(f"    {model_name}_Environment()\n    {{\n")
            h.write(f"        for (int i = 0; i < {len(tex_info)}; i++)\n        {{\n")
            h.write(
                "            displayLists[i] = NULL;\n            dlSizes[i] = 0;\n            textureIDs[i] = 0;\n"
            )
            h.write("        }\n    }\n\n")

            h.write(
                f"    bool load(const char* filepath, const unsigned int* bitmaps[{len(tex_info)}])\n    {{\n"
            )
            h.write(
                '        FILE* file = fopen(filepath, "rb");\n        if (!file) return false;\n\n'
            )
            h.write("        char magic[4];\n        fread(magic, 1, 4, file);\n")
            h.write(
                "        if (magic[0] != 'E' || magic[1] != 'N' || magic[2] != 'V' || magic[3] != '1') { fclose(file); return false; }\n\n"
            )

            h.write(
                "        u32 groupCount;\n        fread(&groupCount, sizeof(u32), 1, file);\n"
            )
            h.write(
                f"        if (groupCount != {len(tex_info)}) {{ fclose(file); return false; }}\n\n"
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
                    f"            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_SIZE_{w}, TEXTURE_SIZE_{h_px}, 0, TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T, bitmaps[{i}]);\n"
                )
                h.write("        }\n")
            h.write("        return true;\n    }\n\n")

            h.write("    void draw()\n    {\n")
            for i in range(len(tex_info)):
                h.write(f"        glBindTexture(GL_TEXTURE_2D, textureIDs[{i}]);\n")
                h.write(
                    f"        if (displayLists[{i}]) glCallList(displayLists[{i}]);\n"
                )
                h.write("        while (GFX_BUSY);\n")
            h.write("    }\n\n")

            h.write("    void cleanup()\n    {\n")
            h.write(f"        for (u32 i = 0; i < {len(tex_info)}; i++)\n        {{\n")
            h.write(
                "            if (displayLists[i]) { free(displayLists[i]); displayLists[i] = NULL; }\n"
            )
            h.write("        }\n")
            h.write(f"        glDeleteTextures({len(tex_info)}, textureIDs);\n")
            h.write("    }\n};\n")

        self.report(
            {"INFO"}, f"Successfully exported {model_name}.bin, .h, and textures."
        )
        return {"FINISHED"}


class VIEW3D_PT_nds_dev_panel(bpy.types.Panel):
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "NDS Engine"
    bl_label = "Production Map Utility"

    def draw(self, context):
        layout = self.layout
        col = layout.column(align=True)
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
    bpy.utils.register_class(OBJECT_OT_nds_bake_vertex_lighting)
    bpy.utils.register_class(EXPORT_SCENE_OT_nds_binary)
    bpy.utils.register_class(VIEW3D_PT_nds_dev_panel)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(OBJECT_OT_nds_bake_vertex_lighting)
    bpy.utils.unregister_class(EXPORT_SCENE_OT_nds_binary)
    bpy.utils.unregister_class(VIEW3D_PT_nds_dev_panel)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()
