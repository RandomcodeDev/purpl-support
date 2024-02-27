local title_id = "0100694203488000"
--local switchhb_mode = "Release"
local devkitpro = os.getenv("DEVKITPRO")

toolchain("switchhb")
    set_kind("standalone")

    set_sdkdir(path.join(devkitpro, "devkitA64"))
    set_bindir(path.join(devkitpro, "devkitA64", "bin"))

    local exe_ext = ""
    if os.host() == "windows" then
        exe_ext = ".exe"
    end

    set_toolset("cc", "aarch64-none-elf-gcc" .. exe_ext)
    set_toolset("cxx", "aarch64-none-elf-g++" .. exe_ext)
    set_toolset("ld", "aarch64-none-elf-gcc" .. exe_ext)
    set_toolset("sh", "aarch64-none-elf-gcc" .. exe_ext)
    set_toolset("ar", "aarch64-none-elf-ar" .. exe_ext)
    set_toolset("as", "aarch64-none-elf-gcc" .. exe_ext)

    on_load(function (toolchain)
        toolchain:add("cxflags", "-fPIC")
        toolchain:add("ldflags", "-mcpu=cortex-a57+fp+simd+crypto+crc")
        toolchain:add("ldflags", "-Wl,-pie")
        toolchain:add("ldflags", "--specs=" .. path.join(devkitpro, "libnx", "switch.specs"))
--        toolchain:add("ldflags", "-Wl,--verbose")

        toolchain:add("cxflags", "-D__unix__")
        toolchain:add("cxflags", "-D__SWITCH__")
        toolchain:add("cxflags", "-D_POSIX_C_SOURCE")

        for _, includedir in ipairs({path.join(devkitpro, "libnx", "include"), path.join(devkitpro, "devkitA64", "aarch64-none-elf", "include")}) do
            toolchain:add("cxflags", "-I" .. includedir)
        end

        for _, libdir in ipairs({path.join(devkitpro, "libnx", "lib"), path.join(devkitpro, "devkitA64", "aarch64-none-elf", "lib")}) do
            toolchain:add("ldflags", "-L" .. libdir)
        end
    end)
toolchain_end()

function switchhb_add_settings()
    add_defines("PURPL_SWITCH_TITLEID=0x" .. title_id)
end

function switchhb_postbuild(target)
    if target:kind() == "binary" then
        local nsp = path.join(target:targetdir(), target:basename() .. "_" .. title_id .. ".nsp")
        local nspd = nsp .. "d"
        os.mkdir(nspd)
        for _, dir in ipairs({"exefs"}) do
            os.mkdir(path.join(nspd, dir))
        end
        for _, pair in ipairs({
            {path.join(path.absolute(target:targetdir()), "assets"), path.join(nspd, "romfs", "assets")}
        }) do
            local source = pair[1]
            local dest = pair[2]
            if os.isfile(source) then
                print("copy " .. source .. " to " .. dest)
                os.cp(source, dest)
            else
                if not os.exists(dest) then
                    print("link " .. source .. " to " .. dest)
                    os.ln(source, dest)
                end
            end
        end

        --os.vrunv(path.join(devkitpro, "tools", "bin", "npdmtool"), {
        --    path.join("platform", "switchhb", "app.json"),
        --    path.join(nspd, "exefs", "main.npdm")
        --})

        os.vrunv(path.join(devkitpro, "tools", "bin", "build_pfs0"), {
            nspd,
            nsp
        })
    end
end
