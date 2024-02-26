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

    set_toolset("cc", "aarch64-unknown-none-elf-gcc" .. exe_ext)
    set_toolset("cxx", "aarch64-unknown-none-elf-g++" .. exe_ext)
    set_toolset("ld", "aarch64-unknown-none-elf-gcc" .. exe_ext)
    set_toolset("ar", "aarch64-unknown-none-elf-ar" .. exe_ext)
    set_toolset("as", "aarch64-unknown-none-elf-as" .. exe_ext)

    on_load(function (toolchain)
        toolchain:add("asflags", "-ffunction-sections")
        toolchain:add("asflags", "-march=armv8-a+crc+crypto")
        toolchain:add("asflags", "-mtune=cortex-a57")
        toolchain:add("asflags", "-mtp=soft")
        toolchain:add("cxflags", "-fPIC")
        toolchain:add("cxflags", "-ffunction-sections")
        toolchain:add("cxflags", "-march=armv8-a+crc+crypto")
        toolchain:add("cxflags", "-mtune=cortex-a57")
        toolchain:add("cxflags", "-mtp=soft")
        toolchain:add("cxxflags", "-fno-rtti")
        toolchain:add("cxxflags", "-fno-exceptions")
        toolchain:add("ldflags", "-Wl,-pie")
        toolchain:add("ldflags", "-T" .. path.join(devkitpro, "libnx", "switch.ld"))
        toolchain:add("ldflags", "-specs=" .. path.join(devkitpro, "libnx", "switch.specs"))

        toolchain:add("cxflags", "-D__unix__")
        toolchain:add("cxflags", "-D__SWITCH__")
        toolchain:add("cxflags", "-D_POSIX_C_SOURCE")

        toolchain:add("ldflags", "-L" .. path.join(devkitpro, "libnx", "lib"))
    end)
toolchain_end()

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
