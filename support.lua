if not is_plat("switch") then
    function add_switch_links() end
    function add_switch_vulkan_links() end
    function switch_postbuild(target) end
    function add_switch_renderapi() end
end

function fix_target(target)
    target:add("options", "mimalloc")
    target:add("options", "verbose")

    if is_plat("gdk", "gdkx") and get_config("toolchain") ~= "mingw" then
        target:set("prefixname", "")
        if target:kind() == "binary" then
            target:set("extension", ".exe")
        elseif target:kind() == "static" then
            target:set("extension", ".lib")
        elseif target:kind() == "shared" then
            target:set("extension", ".dll")
        end
    elseif is_plat("switch") then
        if target:kind() == "binary" then
            target:set("prefixname", "")
            target:set("extension", ".nss")
        elseif target:kind() == "static" then
            target:set("prefixname", "lib")
            target:set("extension", ".a")
        elseif target:kind() == "shared" then
            target:set("prefixname", "lib")
            target:set("extension", ".nrs")
        end
    elseif is_plat("switchhb", "psp", "ps3") then
        if target:kind() == "binary" then
            target:set("prefixname", "")
            target:set("extension", ".elf")
        elseif target:kind() == "static" then
            target:set("prefixname", "lib")
            target:set("extension", ".a")
        elseif target:kind() == "shared" then
            target:set("prefixname", "lib")
            target:set("extension", ".elf")
        end
    elseif not is_plat("windows") then
        -- Of course POSIX or GNU or whoever gets to have "libutil.a" be a reserved name
        -- Other systems don't need this, since they don't pull shit like this
        if target:kind() == "static" then
            target:set("suffixname", "-purpl")
        end
    end
end

function do_settings(support_root)
    set_warnings("everything")

    set_languages("gnu11", "cxx23")
    set_exceptions("cxx")

    add_defines("_CRT_SECURE_NO_WARNINGS")
    add_defines("_GNU_SOURCE")

    if get_config("toolchain") == "msvc" then
        add_cxflags("-Qspectre", "-EHsc", {force = true})
        -- all of these are either external or inconsequential
        add_cxflags(
            "-wd4820", -- padded
            "-wd4365", -- signed", "unsigned mismatch (generally doesn't matter)
            "-wd4255", -- () to (void)
            "-wd4244", -- int to float
            "-wd4464", -- relative include path has .", ".., this is fine because the build is setup right
            "-wd4061", -- enum not explicitly handled by a case label
            "-wd4062",
            "-wd4324", -- padded for alignment specifier
            "-wd4005", -- macro redefinition
            "-wd4668", -- x is not defined as a preprocessor macro, replacing with 0 for #if
            "-wd4113", -- Spectre mitigation
            "-wd5045",
            "-wd4191", -- casting function pointer (used for InitializeMainThread, it doesn't call the pointer)
            "-wd5029", -- nonstandard extension: alignment attributes don't apply to functions
            "-wd4090", -- different const qualifiers (mainly from freeing things that are const)
            "-wd5039", -- exception nonsense (external)
            "-wd4127", -- conditional expression is constant (external)
            "-wd4100", -- unreferenced formal parameter (external)
            "-wd4189", -- local variable is initialized but not referenced (external)
            "-wd4710", -- function not inlined
            "-wd4711", -- function inlined
            "-wd4201", -- nameless struct/union
        {force = true})
        add_cxxflags(
            "-wd5204", -- virtual function something something
            "-wd5027", -- move assignment operator was defined as deleted
            "-wd5026",
            "-wd4626", -- assignment operator was defined as deleted
            "-wd4623", -- default constructor was defined as deleted
            "-wd4625", -- copy constructor was defined as deleted
            "-wd4355", -- this used in base member initializer list
            "-wd4267", -- conversion from size_t to _Ty (in C++ <utility>, not my problem)
            "-wd5267", -- definition of implicit copy constructor is deprecated because it has a user-provided destructor
            "-wd4505", -- unreferenced function with internal linkage has been removed
            "-wd4800", -- implicit conversion to bool, possible information loss
            "-wd5262", -- implicit fallthrough use [[fallthrough]]
            "-wd4388", -- signed/unsigned mismatch
            "-wd5054", -- incompatible enums or'd together
        {force = true})
    elseif get_config("toolchain") == "clang" then
        add_cxflags(
            "-Wno-gnu-line-marker",
            "-Wno-gnu-zero-variadic-macro-arguments",
            "-Wno-extra-semi-stmt",
            "-Wno-padded",
            "-Wno-comma",
            "-Wno-gnu-anonymous-struct",
            "-Wno-bad-function-cast",
            "-Wno-nested-anon-types",
            "-Wno-reserved-identifier",
            "-Wno-unknown-warning-option",
            "-Wno-switch-enum",
            "-Wno-four-char-constants",
            "-Wno-missing-declarations",
            "-Wno-nullability-extension",
            "-Wno-format-nonliteral",
            "-Wno-sign-conversion",
            "-Wno-reserved-identifier",
        {force = true})
        if is_arch("arm64") then
            add_cxflags(
                "-Wno-float-equal",
                "-Wno-double-promotion",
                "-Wno-implicit-int-float-conversion",
            {force = true})
        end
        add_cxxflags(
            "-Wno-c++98-compat",
            "-Wno-c++98-compat-pedantic",
            "-Wno-old-style-cast",
            "-Wno-zero-as-null-pointer-constant",
        {force = true})
    elseif get_config("toolchain") == "switchhb" or get_config("toolchain") == "mingw" then
        add_cxflags(
            "-Wno-unused-value",
            "-Wno-pointer-to-int-cast",
            "-Wno-int-to-pointer-cast",
            "-Wno-frame-address",
            "-Wno-sign-conversion",
            "-Wno-multichar",
            "-Wno-cast-function-type",
        {force = true})
        if is_plat("windows", "gdk", "gdkx") then
            add_defines("_POSIX_C_SOURCE")
        end
    end

    if is_plat("gdk", "gdkx") then
        add_includedirs(
            path.join(os.getenv("GRDKLatest"), "GameKit", "Include")
        )
        add_linkdirs(
            path.join(support_root, "platform", "win32"),
            path.join(os.getenv("GRDKLatest"), "GameKit", "Lib", "amd64")
        )
    end
end

function gdk_postbuild(target)

end

function support_executable(support_root)
    if is_plat("gdk", "gdkx", "windows") then
        add_files(path.join(support_root, "platform", "win32", "launcher.c"))
        if get_config("toolchain") == "msvc" then
            if is_mode("debug") then
                add_ldflags("-subsystem:console", {force = true})
            else
                add_ldflags("-subsystem:windows", {force = true})
            end
        end
        if not is_plat("windows") then
            after_build(gdk_postbuild)
        end
    elseif is_plat("linux", "freebsd") then
        add_files(path.join(support_root, "platform", "unix", "launcher.c"))
    elseif is_plat("switch") then
        add_files(path.join(support_root, "..", "..", "platform", "switch", "launcher.cpp"))
        after_build(switch_postbuild)
    elseif is_plat("switchhb") then
        add_files(path.join(support_root, "platform", "switchhb", "launcher.c"))
        after_build(switchhb_postbuild)
    elseif is_plat("psp") then
        add_files(path.join(support_root, "platform", "psp", "launcher.c"))
        after_build(switchhb_postbuild)
    end
end

function setup_support(support_root, deps_root, use_mimalloc, vulkan, opengl, set_big_settings, config_h_in_path, switch_title_id)
    includes(path.join(support_root, "platform", "switchhb", "switch.lua"))
    includes(path.join(support_root, "platform", "psp", "psp.lua"))
    includes(path.join(support_root, "platform", "ps3", "ps3.lua"))

    if is_plat("windows") then
        add_defines("PURPL_WIN32")
    elseif is_plat("gdk") then
        add_defines("PURPL_WIN32", "PURPL_GDK")
    elseif is_plat("gdkx") then
        add_defines("PURPL_WIN32", "PURPL_GDK", "PURPL_GDKX")
    elseif is_plat("linux") then
        add_defines("PURPL_LINUX", "PURPL_UNIX")
    elseif is_plat("freebsd") then
        add_defines("CmnFreeBSD", "PURPL_UNIX")
    elseif is_plat("switch", "switchhb") then
        add_defines("PURPL_SWITCH", "PURPL_UNIX")
        if is_plat("switchhb") then
            add_defines("PURPL_CONSOLE_HOMEBREW", "PURPL_LEGACY_GRAPHICS")
            switchhb_add_settings(switch_title_id)
        end
    elseif is_plat("psp", "ps3") then
        add_defines("PURPL_PLAYSTATION", "PURPL_UNIX", "PURPL_CONSOLE_HOMEBREW", "PURPL_LEGACY_GRAPHICS")
        if is_plat("psp") then
            add_defines("PURPL_PSP")
        elseif is_plat("ps3") then
            add_defines("PURPL_PS3")
        end
    end

    if get_config("toolchain") == "mingw" then
        add_defines("PURPL_MINGW")
    end

    if is_mode("debug") then
        add_defines("PURPL_DEBUG")
        add_defines('PURPL_BUILD_TYPE="Debug"')
    elseif is_mode("release") then
        add_defines("PURPL_RELEASE")
        add_defines('PURPL_BUILD_TYPE="Release"')
    end

    if is_plat("linux", "freebsd") then
        add_requires("glfw")
    end

    add_includedirs(
        support_root,
        deps_root,
        path.join(deps_root, "cjson"),
        path.join(deps_root, "cglm", "include"),
        path.join(deps_root, "mimalloc", "include"),
        path.join(deps_root, "xxhash"),
        path.join(deps_root, "zstd", "lib"),
        path.absolute(path.join("$(buildir)", "config"))
    )

    if vulkan then
        add_defines("PURPL_VULKAN")

        add_includedirs(
            path.join(deps_root, "Vulkan-Headers", "include"),
            path.join(deps_root, "volk")
        )

        if is_plat("gdk", "windows") then
            add_defines("VK_USE_PLATFORM_WIN32_KHR")
        elseif is_plat("linux", "freebsd") then
            add_defines("VK_USE_PLATFORM_WAYLAND_KHR", "VK_USE_PLATFORM_XCB_KHR")
        elseif is_plat("switch") then
            add_defines("VK_USE_PLATFORM_VI_NN")
        end
    end

    if opengl then
        add_defines("PURPL_OPENGL")

        add_includedirs(
            path.join(deps_root, "glad", "include")
        )

        target("glad")
            set_kind("static")
            add_headerfiles(path.join(deps_root, "glad", "include", "**", "*.h"))
            add_files(path.join(deps_root, "glad", "src", "gl.c"))
            if is_plat("gdk", "windows") then
                add_files(path.join(deps_root, "glad", "src", "wgl.c"))
            else
                add_files(path.join(deps_root, "glad", "src", "egl.c"))
            end
            set_warnings("none")
            set_group("External")
            on_load(fix_target)
        target_end()

        if is_plat("gdk", "windows") then
            add_defines("GLAD_PLATFORM_WIN32")
            add_links("opengl32.lib")
        elseif is_plat("switchhb") then
            add_defines("EGL_NO_X11")
            add_links("EGL", "glapi", "drm_nouveau", "nx")
        end
    end

    if set_big_settings then
        do_settings(support_root)
    end

    target("cjson")
        set_kind("static")
        add_headerfiles(path.join(deps_root, "cjson", "cJSON.h"))
        add_files(path.join(deps_root, "cjson", "cJSON.c"))
        set_warnings("none")
        set_group("External")
        on_load(fix_target)
    target_end()

    if use_mimalloc then
        target("mimalloc")
            set_kind("static")

            add_files(
                path.join(deps_root, "mimalloc", "src", "alloc.c"),
                path.join(deps_root, "mimalloc", "src", "alloc-aligned.c"),
                path.join(deps_root, "mimalloc", "src", "alloc-posix.c"),
                path.join(deps_root, "mimalloc", "src", "arena.c"),
                path.join(deps_root, "mimalloc", "src", "bitmap.c"),
                path.join(deps_root, "mimalloc", "src", "heap.c"),
                path.join(deps_root, "mimalloc", "src", "init.c"),
                path.join(deps_root, "mimalloc", "src", "options.c"),
                path.join(deps_root, "mimalloc", "src", "os.c"),
                path.join(deps_root, "mimalloc", "src", "page.c"),
                path.join(deps_root, "mimalloc", "src", "random.c"),
                path.join(deps_root, "mimalloc", "src", "segment.c"),
                path.join(deps_root, "mimalloc", "src", "segment-map.c"),
                path.join(deps_root, "mimalloc", "src", "stats.c"),
                path.join(deps_root, "mimalloc", "src", "prim", "prim.c")
            )

            if is_plat("windows", "gdk", "gdkx") then
                add_files(path.join(deps_root, "mimalloc", "src", "prim", "windows", "*.c"))
            elseif is_plat("macos") then
                add_files(path.join(deps_root, "mimalloc", "src", "prim", "osx", "*.c"))
            else
                add_files(path.join(deps_root, "mimalloc", "src", "prim", "unix", "*.c"))
            end

            if is_mode("debug") then
                add_defines("MI_DEBUG=3", "MI_SECURE=4")
            end

            set_warnings("none")

            set_group("External")

            add_forceincludes("stdio.h")
            on_load(fix_target)
        target_end()
    end

    target("stb")
        set_kind("static")
        add_files(path.join(deps_root, "stb.c"))
        set_warnings("none")
        set_group("External")
        on_load(fix_target)
    target_end()

    target("xxhash")
        set_kind("static")
        add_headerfiles(
            path.join(deps_root, "xxhash", "xxh3.h"),
            path.join(deps_root, "xxhash", "xxhash.h")
        )
        add_files(path.join(deps_root, "xxhash", "xxhash.c"))
        if is_arch("x86", "x64", "x86_64") then
            add_headerfiles(path.join(deps_root, "xxhash", "xxh_x86dispatch.h"))
            add_files(path.join(deps_root, "xxhash", "xxh_x86dispatch.c"))
        end
        add_defines("XXH_STATIC_LINKING_ONLY")
        set_warnings("none")
        set_group("External")
        on_load(fix_target)
    target_end()

    target("zstd")
        set_kind("static")
        add_headerfiles(path.join(deps_root, "zstd", "lib", "**", "*.h"))
        add_files(path.join(deps_root, "zstd", "lib", "**", "*.c"))
        if is_plat("linux", "freebsd") then
            add_files(path.join(deps_root, "zstd", "lib", "**", "*.S"))
        end
        remove_files(path.join(deps_root, "zstd", "lib", "legacy", "*.c"))
        set_warnings("none")
        set_group("External")
        on_load(fix_target)
    target_end()

    target("common")
        set_kind("static")

        set_configdir(path.join("$(buildir)", "config"))
        set_configvar("USE_MIMALLOC", use_mimalloc and 1 or 0)
        if config_h_in_path ~= nil then
            add_configfiles(config_h_in_path)
            add_headerfiles(config_h_in_path)
        else
            add_configfiles(path.join(support_root, "purpl", "config.h.in"))
            add_headerfiles(path.join(support_root, "purpl", "config.h.in"))
        end

        add_headerfiles(
            path.join(support_root, "common", "*.h"),
            path.join(support_root, "purpl", "*.h"),
            path.join(support_root, "support.lua")
        )
        add_files(path.join(support_root, "common", "*.c"))

        set_group("Support")

        add_deps("platform", "stb", "xxhash", "zstd")
        on_load(fix_target)
    target_end()

    target("platform")
        set_kind("static")
        add_headerfiles(path.join(support_root, "platform", "*.h"))

        if is_plat("gdk", "gdkx", "windows") then
            add_files(
                path.join(support_root, "platform", "win32", "async.c"),
                path.join(support_root, "platform", "win32", "platform.cpp"),
                path.join(support_root, "platform", "win32", "video.c")
            )
            add_links("advapi32", "gdi32", "kernel32", "shell32", "user32")
            if not is_plat("gdkx") then
                add_links("dbghelp")
            end
        elseif is_plat("linux", "freebsd") then
            add_files(
                path.join(support_root, "platform", "unix", "async.c"),
                path.join(support_root, "platform", "unix", "platform.c"),
                path.join(support_root, "platform", "unix", "video.c")
            )
            add_packages("glfw")
            if is_arch("x86") then
                add_links("atomic")
            end
        elseif is_plat("switch") then
            add_headerfiles(path.join(support_root, "..", "..", "platform", "switch", "switch.lua"))
            add_files(
                path.join(support_root, "..", "..", "platform", "switch", "async.cpp"),
                path.join(support_root, "..", "..", "platform", "switch", "platform.cpp"),
                path.join(support_root, "..", "..", "platform", "switch", "video.cpp")
            )
            add_switch_links()
        elseif is_plat("switchhb") then
            add_headerfiles(path.join(support_root, "platform", "switchhb", "switch.lua"))
            add_files(
                path.join(support_root, "platform", "switchhb", "async.c"),
                path.join(support_root, "platform", "switchhb", "platform.c"),
                path.join(support_root, "platform", "switchhb", "video.c")
            )
            add_links("nx")
        elseif is_plat("psp") then
            add_headerfiles(path.join(support_root, "platform", "psp", "psp.lua"))
            add_files(
                path.join(support_root, "platform", "psp", "async.c"),
                path.join(support_root, "platform", "psp", "platform.c"),
                path.join(support_root, "platform", "psp", "video.c")
            )
        end

        if use_mimalloc then
            add_deps("mimalloc")
        end

        if opengl then
            add_deps("glad")
        end

        set_group("Support")

        on_load(fix_target)
    target_end()

    target("util")
        set_kind("static")
        add_headerfiles(path.join(support_root, "util", "*.h"))
        add_files(path.join(support_root, "util", "*.c"))
        add_deps("cjson", "common", "zstd")
        set_group("Support")
        on_load(fix_target)
    target_end()
end
