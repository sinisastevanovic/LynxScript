workspace "LynxScript"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    startproject "LynxScript"

    flags { "MultiProcessorCompile" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        optimize "On"

project "LynxScript"
    kind "ConsoleApp"
    language "C"
    cdialect "C99"
    architecture "x86_64"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin-int/%{cfg.buildcfg}"

    includedirs { "src" }
    files { "src/**.h", "src/**.c" }