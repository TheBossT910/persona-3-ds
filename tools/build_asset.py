#!/usr/bin/env python3
import sys
import os
import json
import importlib
import argparse
from typing import Optional


def load_config(input_path: str) -> dict:
    """
    Load configuration from a .build.json sidecar file.

    Search order:
      1. <input_dir>/<folder_name>.build.json   (sidecar inside the asset folder)
      2. <parent_dir>/<folder_name>.build.json  (sidecar adjacent to the asset folder)
      3. <input_base>.build.json                (sidecar next to a single input file)
      4. <grandparent_dir>/<stem>.build.json    (legacy fallback)
    """
    input_path = os.path.abspath(input_path)

    candidates = []

    if os.path.isdir(input_path):
        dir_name = os.path.basename(os.path.normpath(input_path))
        parent_dir = os.path.dirname(input_path)
        candidates = [
            os.path.join(input_path, f"{dir_name}.build.json"),  # inside folder
            os.path.join(parent_dir, f"{dir_name}.build.json"),  # adjacent to folder
        ]
    else:
        base = os.path.splitext(input_path)[0]
        parent_dir = os.path.dirname(input_path)
        grandparent = os.path.dirname(parent_dir)
        stem = os.path.basename(base)
        candidates = [
            base + ".build.json",  # next to the file
            os.path.join(grandparent, stem + ".build.json"),  # legacy fallback
        ]

    for path in candidates:
        if os.path.exists(path):
            print(f"[build_asset] Config loaded: {path}")
            with open(path, "r") as f:
                return json.load(f)

    print("[build_asset] No .build.json found; using empty config.")
    return {}


def guess_asset_type(input_path: str, config: dict) -> Optional[str]:
    """Determine the asset type from config, directory contents, or file extension."""

    # Explicit override in JSON always wins
    if config.get("asset_type"):
        return config["asset_type"]

    # Directory heuristic: presence of a .bin means it's an environment
    if os.path.isdir(input_path):
        contents = os.listdir(input_path)
        if any(f.lower().endswith(".bin") for f in contents):
            return "build_environment"

    ext = os.path.splitext(input_path)[1].lower()
    ext_map = {
        ".dlg": "dlg2dialogue",
        ".mp4": "video2vid",
        ".jmap": "jmap2map",
        ".json": "build_model",
        ".bin": "build_environment",
    }
    return ext_map.get(ext)


def main() -> None:
    parser = argparse.ArgumentParser(description="Asset Compiler")
    parser.add_argument("input", help="Input file or directory")
    parser.add_argument("output", help="Output file or directory")
    args, unknown = parser.parse_known_args()

    config = load_config(args.input)
    asset_type = guess_asset_type(args.input, config)

    if not asset_type:
        print(f"Error: Cannot determine asset type for {args.input}")
        sys.exit(1)

    print(f"[build_asset] asset_type = {asset_type}")

    # Merge unknown CLI flags into config (--key value pairs)
    i = 0
    while i < len(unknown):
        if unknown[i].startswith("--"):
            key = unknown[i][2:].replace("-", "_")
            vals = []
            i += 1
            while i < len(unknown) and not unknown[i].startswith("--"):
                raw = unknown[i]
                try:
                    val = float(raw) if "." in raw else int(raw)
                except ValueError:
                    val = raw
                vals.append(val)
                i += 1

            if len(vals) == 0:
                config[key] = True
            elif len(vals) == 1:
                config[key] = vals[0]
            else:
                config[key] = vals
        else:
            i += 1

    try:
        converter = importlib.import_module(f"converters.{asset_type}")
    except ModuleNotFoundError:
        print(f"Error: Module 'converters.{asset_type}' not found.")
        sys.exit(1)

    converter.convert(args.input, args.output, config)


if __name__ == "__main__":
    main()
