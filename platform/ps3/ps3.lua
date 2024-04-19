local ps3root = os.getenv("PS3DEV")

toolchain("ps3")
    set_kind("standalone")

    set_sdkdir(path.join(ps3root, "ppu"))
    set_bindir(path.join(ps3root, "ppu", "bin"))

    set_toolset("cc", "ppu-gcc")
    set_toolset("cxx","ppu-g++")
    set_toolset("ld", "ppu-g++")
    set_toolset("sh", "ppu-g++")
    set_toolset("ar", "ppu-ar")
    set_toolset("as", "ppu-gcc")

    on_load(function (toolchain)
        toolchain:add("cxflags", "-D_POSIX_C_SOURCE")
        toolchain:add("cxflags", "-D_POSIX_THREADS")

        for _, includedir in ipairs({
            path.join(ps3root, "include"),
            path.join(ps3root, "ppu", "powerpc64-ps3-elf", "include")
        }) do
            toolchain:add("cxflags", "-I" .. includedir)
        end

        for _, libdir in ipairs({
            path.join(ps3root, "lib"),
            path.join(ps3root, "ppu", "powerpc64-ps3-elf", "lib")
        }) do
            toolchain:add("ldflags", "-L" .. libdir)
        end
    end)
toolchain_end()

function ps3_add_settings(support_root, deps_root)
    add_includedirs(path.join(deps_root, "rsxgl", "include"))
    target("rsxgl")
        set_kind("static")
        add_headerfiles(path.join(deps_root, "rsxgl", "include", "**.h"))
        add_files(path.join(deps_root, "rsxgl", "src", "**.c"), path.join(deps_root, "rsxgl", "src", "**.cc"))
    target_end()
end

function ps3_postbuild(target)
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
