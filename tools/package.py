#!/usr/bin/env python3

import glob
import os
import shutil
import sys

def main():
    platform = sys.argv[1]
    architecture = sys.argv[2]
    configuration = sys.argv[3]

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

    for name in files:
        path = os.path.join(build, name)
        for file in glob.glob(path):
            target = os.path.join(output, os.path.dirname(name))
            if os.path.exists(target):
                if os.path.isdir(file):
                    shutil.rmtree(target)
                else:
                    shutil.delete(target)
            os.makedirs(target)
            print(f"{file} -> {target}")
            if os.path.isdir(file):
                shutil.copytree(file, os.path.join(target, name))
            else:
                shutil.copy(file, target)

if __name__ == "__main__":
    main()
