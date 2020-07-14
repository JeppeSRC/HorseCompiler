workspace("HorseCompiler")
    location "solution/"
    cppdialect "c++17"
    startproject "Test"

    configurations {
        "Release",
        "Debug"
    }

    platforms "x64"
    architecture "x64"
    callingconvention "FastCall"

    defines "_CRT_SECURE_NO_WARNINGS"

    flags {
        "NoPCH"
    }

    filter("Debug") 
        defines "HC_DEBUG"
        optimize "Off"
        inlining "Disabled"

    filter("Release")
        defines "HC_RELEASE"
        flags "LinkTimeOptimization"    
        optimize "Speed"
        inlining "Auto"

    filter ""
    
    project("HorseCompiler")
        location "solution/HorseCompiler/"
        targetdir "%{sln.location}/../bin/HorseCompiler/%{cfg.buildcfg}/"
        objdir "%{sln.location}/../bin/HorseCompiler/%{cfg.buildcfg}/intermediates/"

        files {
            "src/HorseCompiler/**.h",
            "src/HorseCompiler/**.c",
            "src/HorseCompiler/**.cpp"
        }

        includedirs "src/HorseCompiler/"

        kind "StaticLib"

    project("Test")
        location "solution/Test/"
        targetdir "%{sln.location}/../bin/Test/%{cfg.buildcfg}/"
        objdir "%{sln.location}/../bin/Test/%{cfg.buildcfg}/intermediates/"
        
        kind "ConsoleApp"

        dependson "HorseCompiler"
        links "HorseCompiler"
        
        includedirs "src/HorseCompiler/"
        includedirs "src/Test/"

        files {
            "src/Test/**.h",
            "src/Test/**.c",
            "src/Test/**.cpp"
        }