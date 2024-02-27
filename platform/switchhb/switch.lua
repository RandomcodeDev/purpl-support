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
    set_toolset("ld", "aarch64-none-elf-g++" .. exe_ext)
    set_toolset("sh", "aarch64-none-elf-gcc" .. exe_ext)
    set_toolset("ar", "aarch64-none-elf-ar" .. exe_ext)
    set_toolset("as", "aarch64-none-elf-gcc" .. exe_ext)

    on_load(function (toolchain)
        toolchain:add("asflags", "-fPIE")
        toolchain:add("cxflags", "-fPIE")
        toolchain:add("cxflags", "-ffunction-sections")
        toolchain:add("cxflags", "-mtp=soft")
        toolchain:add("ldflags", "-mcpu=cortex-a57+fp+simd+crypto+crc")
        toolchain:add("ldflags", "--specs=" .. path.join(devkitpro, "libnx", "switch.specs"))
        toolchain:add("ldflags", "-Wl,--warn-textrel")

        toolchain:add("cxflags", "-D__unix__")
        toolchain:add("cxflags", "-D__SWITCH__")
        toolchain:add("cxflags", "-D_POSIX_C_SOURCE")

        for _, includedir in ipairs({
            path.join(devkitpro, "libnx", "include"),
            path.join(devkitpro, "devkitA64", "aarch64-none-elf", "include"),
            path.join(devkitpro, "portlibs", "switch", "include")
        }) do
            toolchain:add("cxflags", "-I" .. includedir)
        end

        for _, libdir in ipairs({
            path.join(devkitpro, "libnx", "lib"),
            path.join(devkitpro, "devkitA64", "aarch64-none-elf", "lib"),
            path.join(devkitpro, "portlibs", "switch", "lib")
        }) do
            toolchain:add("ldflags", "-L" .. libdir)
        end
    end)
toolchain_end()

function switchhb_add_settings(title_id)
    add_defines("PURPL_SWITCH_TITLEID=0x" .. title_id)
end

function switchhb_postbuild(target)
    if target:kind() == "binary" then
        local nro = path.join(path.absolute(target:targetdir()), target:basename() .. ".nro")
        local nacp = path.join(path.absolute(target:targetdir()), "control.nacp")
        local romfs = path.join(path.absolute(target:targetdir()), "romfs")
        os.mkdir(romfs)
        for _, pair in ipairs({
            {path.join(path.absolute(target:targetdir()), "assets"), path.join(romfs, "assets")}
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

        local support_data = target:get("support_data")
        local app_name = support_data[1]
        local app_author = support_data[2]
        local title_id = support_data[3]

        os.vrunv(path.join(devkitpro, "tools", "bin", "nacptool"), {
            "--create",
            app_name,
            app_author,
            target:get("version"),
            nacp,
            "--titleid=" .. title_id
        })

        os.vrunv(path.join(devkitpro, "tools", "bin", "elf2nro"), {
            path.absolute(target:targetfile()),
            nro,
            "--nacp=" .. nacp,
            "--icon=" .. path.absolute(path.join(path.absolute(target:scriptdir()), "assets", "switchhb", "icon.jpg")),
            "--romfsdir=" .. romfs
        })
    end
end
