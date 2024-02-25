add_rules(
    "mode.debug",
    "mode.release",
    "plugin.vsxmake.autoupdate"
)

set_project("purpl-support")

set_version("0.0.0", {build = "%Y%m%d%H%M"})

includes("support.lua")
setup_support(".", "deps", true, true)
