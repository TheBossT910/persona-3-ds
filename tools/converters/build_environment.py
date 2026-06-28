import os
import shutil


def convert(input_path, output_dir, config):
    """
    Routes .bin and .h to their build directories and generates .grit files for textures.
    Reads grit_flags from the .build.json sidecar config.
    """
    # Resolve the source directory whether input is a file or folder
    input_path = os.path.abspath(input_path)
    if os.path.isdir(input_path):
        src_dir = input_path
    elif os.path.isfile(input_path):
        src_dir = os.path.dirname(input_path)
    else:
        raise FileNotFoundError(f"CRITICAL: Input path does not exist: {input_path}")

    folder_name = os.path.basename(os.path.normpath(src_dir))

    # Destination directories (relative to project root / cwd)
    data_env_dir = os.path.abspath(os.path.join("data", "environments", folder_name))
    source_env_dir = os.path.abspath(os.path.join("source", "environments"))

    os.makedirs(data_env_dir, exist_ok=True)
    os.makedirs(source_env_dir, exist_ok=True)

    # Pull grit flags from .build.json, or fall back to safe defaults.
    # The JSON value can be either a single string or a list of flag strings.
    raw_flags = config.get("grit_flags", ["-gT!", "-gB16", "-gb", "-ftb"])
    if isinstance(raw_flags, str):
        # Allow the JSON to store flags as a single space-separated string
        flag_list = raw_flags.split()
    else:
        flag_list = list(raw_flags)

    # .grit format: one flag per line
    grit_content = "\n".join(flag_list) + "\n"

    found_bin = False
    found_header = False

    print(f"[build_environment] Scanning: {src_dir}")
    print(f"  data  -> {data_env_dir}")
    print(f"  source-> {source_env_dir}")
    print(f"  grit flags: {' '.join(flag_list)}")

    for file_name in sorted(os.listdir(src_dir)):
        file_path = os.path.join(src_dir, file_name)
        if not os.path.isfile(file_path):
            continue

        lower = file_name.lower()

        # 1. Route the 3D map binary (.bin) -> data/environments/<folder>/
        if lower.endswith(".bin"):
            found_bin = True
            dest = os.path.join(data_env_dir, file_name)
            shutil.copy2(file_path, dest)
            print(f"  [BIN]    {file_path}")
            print(f"        -> {dest}")

        # 2. Route the C++ header (.h) -> source/environments/
        elif lower.endswith(".h"):
            found_header = True
            dest = os.path.join(source_env_dir, file_name)
            shutil.copy2(file_path, dest)
            print(f"  [HEADER] {file_path}")
            print(f"        -> {dest}")

        # 3. Generate a .grit sidecar next to each texture (.png)
        elif lower.endswith(".png"):
            grit_path = os.path.splitext(file_path)[0] + ".grit"
            with open(grit_path, "w") as f:
                f.write(grit_content)
            print(f"  [GRIT]   {grit_path}")

    # Hard fail if the geometry binary is missing
    if not found_bin:
        raise FileNotFoundError(
            f"CRITICAL: No .bin file found in {src_dir}\n"
            "Make sure the Blender export completed successfully."
        )

    if not found_header:
        print(f"  [WARN] No .h file found in {src_dir} — skipping header copy.")
