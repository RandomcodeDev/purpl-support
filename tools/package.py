#!/usr/bin/env python3

import glob
import os
import shutil
import sys

def main():
    if len(sys.argv) < 4:
        print("package.py <platform> <architecture> <configuration>")
        exit(1)

    platform = sys.argv[1]
    architecture = sys.argv[2]
    configuration = sys.argv[3]

    print(f"packaging for {platform} {architecture} {configuration}")

    build = os.path.join("build", platform, architecture, configuration)
    output = os.path.join(build, "package")

    files = ["assets_*.pak"]
    if platform in ["gdk", "gdkx", "windows"]:
        files += [
            "purpl.exe",
            "purpl.exe.manifest",
        ]

        if platform in ["gdk", "gdkx"]:
            files += ["*.sym", "GdkAssets", "MicrosoftGame.Config"]
        else:
            files += ["*.pdb"]
    elif platform in ["linux"]:
        files += [
            "purpl"
        ]
    elif platform in ["switch", "switchhb"]:
        files = [
            "purpl_0100694203488000.nsp"
        ]

    if os.path.exists(output):
        print(f"removing {output}")
        shutil.rmtree(output)

    for name in files:
        path = os.path.join(build, name)
        globbed = glob.glob(path)
        if len(globbed) < 1:
            globbed = [path]
        print(f"files: {globbed}")
        for file in globbed:
            name = f"{os.path.basename(file)}"
            target = os.path.join(output, os.path.dirname(name))
            if not os.path.exists(target):
                os.makedirs(target)
            print(f"{file} -> {target}")
            if os.path.isdir(file):
                shutil.copytree(file, os.path.join(target, name))
            else:
                shutil.copy(file, os.path.join(target, name))

if __name__ == "__main__":
    main()
