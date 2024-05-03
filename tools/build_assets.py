#!/usr/bin/env python3

import argparse
import os
import platform
import shutil

from os import PathLike
from subprocess import Popen, PIPE
from types import *
from typing import *

if platform.system() == "Windows":
    exe_ext = ".exe"
else:
    exe_ext = ""

assets_dir = ""
output_dir = ""
tools_dir = ""
repo_tools_dir = ""
repo_tools_native_dir = ""

dxc = ""
spirv_cross = ""
meshtool = ""
texturetool = ""
packtool = ""

verbose = False
rebuild = False
purge = False


def main():
    def parse_arguments():
        def find_tools():
            env_dir = os.getenv("PURPL_TOOLS_DIR")
            if env_dir != None and os.path.exists(env_dir):
                return env_dir

            guess_dir = os.path.abspath(
                os.path.join(
                    os.path.dirname(__file__),
                    "..",
                    "build",
                    platform.system().lower(),
                    platform.machine().replace("AMD64", "x64"),
                    "release",
                )
            )
            print(f"Trying to find tools in {guess_dir}")
            if os.path.exists(guess_dir):
                return guess_dir

            return None

        parser = argparse.ArgumentParser(description="Your program description here.")
        parser.add_argument(
            "-a",
            "--assets-dir",
            default=os.path.abspath(
                os.path.join(os.path.dirname(__file__), "..", "..", "assets")
            ),
            help="Path to the assets directory (default: assets directory two above the script directory)",
        )
        parser.add_argument(
            "-o",
            "--output-dir",
            default=os.path.join(
                os.path.abspath(os.path.dirname(__file__)), "..", "..", "assets", "out"
            ),
            help="Path to the output directory (default: assets_dir/out)",
        )
        parser.add_argument(
            "-R",
            "--repo-tools-dir",
            default=os.path.abspath(
                os.path.join(os.path.dirname(__file__), "..", "tools")
            ),
            help="Path to the Purpl repository's tools (default: tools folder in the parent directory of the script directory)",
        )
        parser.add_argument(
            "-t",
            "--tools-dir",
            default=find_tools(),
            help="Path to the tools directory (default: value of PURPL_TOOLS_DIR environment variable)",
        )
        parser.add_argument(
            "-v",
            "--verbose",
            action="store_true",
            default=False,
            help="Whether to print tool output (default: false)",
        )
        parser.add_argument(
            "-r",
            "--rebuild",
            action="store_true",
            default=False,
            help="Whether to rebuild assets (default: false)",
        )
        parser.add_argument(
            "-p",
            "--purge",
            action="store_true",
            default=False,
            help="Whether to purge the output directory (default: false)",
        )

        return parser.parse_args()

    def build_asset(function: FunctionType, src: List[PathLike], dest: PathLike):
        os.makedirs(os.path.dirname(dest), exist_ok=True)
        dest_exists = os.access(dest, os.F_OK)
        if dest_exists:
            src_stat = os.stat(src)
            dest_stat = os.stat(dest)
            newer = src_stat.st_mtime > dest_stat.st_mtime
        else:
            newer = False
        if rebuild or (newer and dest_exists) or not dest_exists:
            command = function(src, dest)
            print(f"{command} (newer: {newer}, dest_exists: {dest_exists})")
            os.chmod(command[0], 755)
            process = Popen(
                command,
                stdout=PIPE,
                stderr=PIPE,
                env={"LD_LIBRARY_PATH": repo_tools_native_dir},
            )
            stdout, stderr = process.communicate()
            stdout = bytes.decode(stdout, encoding="utf-8")
            stderr = bytes.decode(stderr, encoding="utf-8")
            if verbose:
                print(f"Output:\n{stdout}\n{stderr}")
            if process.returncode != 0:
                print(f"building {src} failed: exit code {process.returncode}")
                exit(1)
        else:
            print(f"Skipping {dest} (newer: {newer}, dest_exists: {dest_exists})")

    args = parse_arguments()
    assets_dir = args.assets_dir
    output_dir = args.output_dir
    tools_dir = args.tools_dir
    repo_tools_dir = args.repo_tools_dir
    repo_tools_native_dir = os.path.join(
        repo_tools_dir,
        platform.system().lower(),
        platform.machine().replace("AMD64", "x64"),
    )
    if repo_tools_dir == None:
        print(
            "One or more tool directories not defined, can't continue\n"
            + "You might not have built the tools, check the README for instructions"
        )
        exit(1)
    verbose = args.verbose
    rebuild = args.rebuild
    purge = args.purge

    print(
        f"{'(Re)b' if rebuild else 'B'}uilding assets in {assets_dir} with tools in {tools_dir}, {repo_tools_dir}, and {repo_tools_native_dir}"
    )

    def find_tool(dir: PathLike, name: AnyStr) -> str:
        path = os.path.join(dir, name + exe_ext)
        if not os.path.exists(path):
            print(f"Missing {path}, did you follow the instructions?")
            exit(1)
        return path

    dxc = find_tool(repo_tools_native_dir, "dxc")
    spirv_cross = find_tool(repo_tools_native_dir, "spirv-cross")
    meshtool = find_tool(tools_dir, "meshtool")
    texturetool = find_tool(tools_dir, "texturetool")
    packtool = find_tool(tools_dir, "packtool")

    if purge:
        print(f"Purging {output_dir}")
        shutil.rmtree(output_dir)

    for root, _, files in os.walk(os.path.join(assets_dir, "models")):
        for file in files:
            build_asset(
                lambda src, dest: [meshtool, "to", src, dest],
                os.path.join(root, file),
                os.path.join(
                    root.replace(assets_dir, output_dir),
                    os.path.splitext(file)[0] + ".pmdl",
                ),
            )

    for root, _, files in os.walk(os.path.join(assets_dir, "textures")):
        for file in files:
            build_asset(
                lambda src, dest: [texturetool, "to", src, dest],
                os.path.join(root, file),
                os.path.join(
                    root.replace(assets_dir, output_dir),
                    os.path.splitext(file)[0] + ".ptex",
                ),
            )

    for root, _, files in os.walk(os.path.join(assets_dir, "shaders")):
        for file in files:
            if file.endswith(".hlsl"):
                name = os.path.splitext(file)[0]
                for shader in [
                    ("VertexMain", "vs_6_0", ["-fvk-invert-y"]),
                    ("PixelMain", "ps_6_0", []),
                ]:
                    entry = shader[0]
                    model = shader[1]
                    vkflags = shader[2]
                    extension = model[0:2]
                    build_asset(
                        lambda src, dest: [
                            dxc,
                            "-E",
                            entry,
                            "-T",
                            model,
                            src,
                            "-Fo",
                            dest,
                        ],
                        os.path.join(root, file),
                        os.path.join(
                            root.replace(assets_dir, output_dir),
                            "directx12",
                            f"{name}.{extension}.cso",
                        ),
                    )
                    build_asset(
                        lambda src, dest: [
                            dxc,
                            "-E",
                            entry,
                            "-T",
                            model,
                            "-spirv",
                            "-DSPIRV",
                            src,
                            "-Fo",
                            dest,
                        ] + vkflags,
                        os.path.join(root, file),
                        os.path.join(
                            root.replace(assets_dir, output_dir),
                            "vulkan",
                            f"{name}.{extension}.spv",
                        ),
                    )
                    build_asset(
                        lambda src, dest: [spirv_cross, src, "--output", dest],
                        os.path.join(
                            root.replace(assets_dir, output_dir),
                            "vulkan",
                            f"{name}.{extension}.spv",
                        ),
                        os.path.join(
                            root.replace(assets_dir, output_dir),
                            "opengl",
                            f"{name}.{extension}.glsl",
                        ),
                    )

    if os.path.exists(os.path.join(assets_dir, "fonts")):
        try:
            shutil.copytree(
                os.path.join(assets_dir, "fonts"), os.path.join(output_dir, "fonts")
            )
        except FileExistsError:
            pass

    if os.path.exists(os.path.join(assets_dir, "assets_dir.pak")):
        for item in os.listdir(assets_dir):
            if item.endswith(".pak"):
                os.remove(os.path.join(assets_dir, item))

    build_asset(
        lambda src, dest: [packtool, "create", dest.replace("_dir.pak", ""), src],
        output_dir,
        os.path.join(assets_dir, "assets_dir.pak")
    )


if __name__ == "__main__":
    main()
