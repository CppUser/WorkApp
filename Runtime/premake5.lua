project "Runtime"
    kind "ConsoleApp"
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
        "src/private/**.c"
    }

    includedirs {
        "src/",
        "src/public/",
        "src/private/",
        "%{wks.location}/Core/src/public"
    }

    links { "Core" }
    dependson { "Core" }

    IncludeDependencies()

    filter "system:windows"
        systemversion "latest"

        postbuildcommands {
            -- Copy DLLs like tdjson.dll, libcrypto, etc.
            '{COPY} "%{wks.location}/Core/tplibs/tdlib/bin/tdjson.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/Core/tplibs/tdlib/bin/libcrypto-3-x64.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/Core/tplibs/tdlib/bin/libssl-3-x64.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/Core/tplibs/tdlib/bin/zlib1.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/Core/tplibs/glfw/lib/glfw3.dll" "%{cfg.targetdir}"'
        }

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
