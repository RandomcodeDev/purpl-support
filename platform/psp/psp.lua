local pspdev = os.getenv("PSPDEV")

toolchain("psp")
    set_kind("standalone")

    set_sdkdir(pspdev)
    set_bindir(path.join(pspdev, "bin"))

    set_toolset("cc", "psp-gcc")
    set_toolset("cxx", "psp-g++")
    set_toolset("ld", "psp-g++")
    set_toolset("sh", "psp-gcc")
    set_toolset("ar", "psp-ar")
    set_toolset("as", "psp-gcc")

    on_load(function (toolchain)
        toolchain:add("cxflags", "-D_POSIX_C_SOURCE")

        for _, includedir in ipairs({
            path.join(pspdev, "include"),
            path.join(pspdev, "psp", "include"),
            path.join(pspdev, "psp", "sdk", "include")
        }) do
            toolchain:add("cxflags", "-I" .. includedir)
        end

        for _, libdir in ipairs({
            path.join(pspdev, "lib"),
            path.join(pspdev, "psp", "lib"),
            path.join(pspdev, "psp", "sdk", "lib")
        }) do
            toolchain:add("ldflags", "-L" .. libdir)
        end
    end)
toolchain_end()

function psp_add_settings()
end

function psp_postbuild(target)
    if target:kind() == "binary" then
        local prx = path.join(path.absolute(target:targetdir()), target:basename() .. ".prx")
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

        os.vrunv(path.join(pspdev, "bin", "psp-prxgen"), {
            path.absolute(target:targetfile()),
            prx
        })
    end
end
