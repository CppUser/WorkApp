#include "%{wks.location}/Dependencies.lua"

project "Core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++23"
    staticruntime "off"

    targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
    objdir    ("%{wks.location}/build/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/public/**.h",
        "src/public/**.hpp",
        "src/public/**.inl",
        "src/private/**.cpp",
        "src/private/**.c",

        -- GLAD + IMGUI sources
        "%{Dependencies.Glad.Src}",
        table.unpack(Dependencies.ImGui.Src)
    }

    includedirs {
        "src/",
        "src/public/",
        "src/private/",
    }

    IncludeDependencies()

    filter "system:windows"
        systemversion "latest"
        defines { "GLFW_INCLUDE_NONE" }

    filter "configurations:Debug"
        defines { "_DEBUG" }
        runtime "Debug"
        symbols "On"
        ProcessDependencies("Debug")

    filter "configurations:Release"
        defines { "_RELEASE" }
        runtime "Release"
        optimize "Full"
        symbols "Off"
        ProcessDependencies("Release")
