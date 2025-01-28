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
    --kind "ConsoleApp"
    kind "StaticLib"
    language "C"
    cdialect "C99"
    architecture "x86_64"

    --targetdir "bin/%{cfg.buildcfg}"
    targetdir "lib/%{cfg.buildcfg}"
    objdir "bin-int/%{cfg.buildcfg}"

    includedirs { "src", "include" }
    files { "src/**.h", "src/**.c", "include/**.h", "include/**.hpp", "include/**.c" }