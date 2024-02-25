add_rules(
    "mode.debug",
    "mode.release",
    "plugin.vsxmake.autoupdate"
)

set_project("purpl-support")

set_version("0.0.0", {build = "%Y%m%d%H%M"})

function fix_target(target)
    target:add("options", "mimalloc")
    target:add("options", "verbose")

    if is_plat("gdk", "gdkx") then
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
    elseif not is_plat("windows") then
        -- Of course POSIX or GNU or whoever gets to have "libutil.a" be a reserved name
        -- Other systems don't need this, since they don't pull shit like this
        if target:kind() == "static" then
            target:set("suffixname", "-purpl")
        end
    end
end

includes("support.lua")
setup_support(".", "deps", true, true, fix_target)
