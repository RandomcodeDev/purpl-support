target("meshtool")
    set_kind("binary")
    add_files("meshtool.c")
    add_deps("common", "platform", "util")
    add_packages("assimp")
target_end()

target("packtool")
    set_kind("binary")
    add_files("packtool.c")
    add_deps("common", "platform")
    if is_plat("windows") then
        add_packages("dirent")
    end
target_end()

target("texturetool")
    set_kind("binary")
    add_files("texturetool.c")
    add_deps("common", "platform", "util")
target_end()
