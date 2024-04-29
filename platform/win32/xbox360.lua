local xbox360root = os.getenv("XEDK")

toolchain("xbox360")
    set_kind("standalone")

    set_sdkdir(path.join(xbox360root))
    set_bindir(path.join(xbox360root, "bin"))

    set_toolset("cc", path.join(xbox360root, "bin", "win32", "cl.exe"))
    set_toolset("cxx",path.join(xbox360root, "bin", "win32", "cl.exe"))
    set_toolset("ld", path.join(xbox360root, "bin", "win32", "link.exe"))
    set_toolset("sh", path.join(xbox360root, "bin", "win32", "link.exe"))
    set_toolset("ar", path.join(xbox360root, "bin", "win32", "link.exe"))
    set_toolset("as", path.join(xbox360root, "bin", "win32", "ml.exe"))

    on_load(function (toolchain)
        toolchain:add("cflags", "-TC")
        toolchain:add("cxxflags", "-TP")
        toolchain:add("cxflags", "-X")
        toolchain:add("cxflags", "-Dalignof=__alignof")
        toolchain:add("cxflags", "-D_XBOX")
        toolchain:add("ldflags", "-nodefaultlib")
        toolchain:add("ldflags", "-xex:no")

        for _, includedir in ipairs({
            path.join(xbox360root, "include", "xbox"),
            path.join(xbox360root, "Source", "crt")
        }) do
            toolchain:add("includedirs", includedir)
        end

        for _, libdir in ipairs({
            path.join(xbox360root, "lib", "xbox")
        }) do
            toolchain:add("ldflags", "-libdir:" .. libdir)
        end
    end)
toolchain_end()

function xbox360_add_settings(support_root, deps_root)
    add_forceincludes(path.absolute(path.join(support_root, "platform", "win32", "360intrin.h")))
end

function xbox360_postbuild(target)
    if target:kind() == "binary" then
        --local nacp = path.join(path.absolute(target:targetdir()), "control.nacp")
        --local romfs = path.join(path.absolute(target:targetdir()), "romfs")
        --os.mkdir(romfs)
        --for _, pair in ipairs({
        --    {path.join(path.absolute(target:targetdir()), "assets"), path.join(romfs, "assets")}
        --}) do
        --    local source = pair[1]
        --    local dest = pair[2]
        --    if os.isfile(source) then
        --        print("copy " .. source .. " to " .. dest)
        --        os.cp(source, dest)
        --    else
        --        if not os.exists(dest) then
        --            print("link " .. source .. " to " .. dest)
        --            os.ln(source, dest)
        --        end
        --    end
        --end
    end
end
