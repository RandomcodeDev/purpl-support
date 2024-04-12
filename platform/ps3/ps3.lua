local ps3root = os.getenv("SCE_PS3_ROOT")

toolchain("ps3")
    set_kind("standalone")

    set_sdkdir(path.join(ps3root, "host-win32", "ppu"))
    set_bindir(path.join(ps3root, "host-win32", "ppu", "bin"))

    set_toolset("cc", "clang")
    set_toolset("cxx","clang++")
    set_toolset("ld", "clang++")
    set_toolset("sh", "ppu-lv2-gcc")
    set_toolset("ar", "ppu-lv2-ar")
    set_toolset("as", "ppu-lv2-gcc")

    on_load(function (toolchain)
        toolchain:add("cxflags", "-D_POSIX_C_SOURCE")
        toolchain:add("cxflags", "--target=powerpc64-none-elf")

        for _, includedir in ipairs({
            path.join(ps3root, "target", "ppu", "include")
        }) do
            toolchain:add("cxflags", "-I" .. includedir)
        end

        for _, libdir in ipairs({
            path.join(ps3root, "target", "ppu", "lib")
        }) do
            toolchain:add("ldflags", "-L" .. libdir)
        end
    end)
toolchain_end()

function ps3_add_settings()
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
