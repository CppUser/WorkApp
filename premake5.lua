local dependencies_path = path.join(os.getcwd(), "Dependencies.lua")
print("Loading dependencies from: " .. dependencies_path)

if os.isfile(dependencies_path) then
    dofile(dependencies_path)
    print("Dependencies.lua loaded successfully")
else
    print("ERROR: Dependencies.lua not found at: " .. dependencies_path)
    error("Dependencies.lua not found. Please make sure it exists in the scripts directory.")
end

workspace "WorkApp" do
    configurations {"Debug", "Release"}
    architecture "x86_64"
    targetdir "build"
    startproject "Runtime"

    language "C++"
    cppdialect "C++23"
    flags {"MultiProcessorCompile"}

    filter "system:windows"
        systemversion "latest"
        --W4 or Wall warning levels
        buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" , "/utf-8" , "/std:c++latest"}

    filter "configurations:Debug"
        optimize "Off"
        symbols "On"
        --/Zi or /Z7 for debug info
        --/RTC1 for runtime check (uninitialied vars  stack frame etc)
        --/analyze static analysis ?
        buildoptions {"/fp:precise"}

    filter "configurations:Release"
        optimize "On"
        symbols "Default"

        --GL - whole prog optimization (link time optimization )
        --/arch:AVX2 vector instructions if modern hardware will be used for game
        --/fp:fast  faster floating point math
        buildoptions {"/O2","/fp:fast", "/GL"} 
        linkoptions {"/LTCG"}

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    bindir = "%{wks.location}/build/bin/" .. outputdir
end

group "Core" do
    include "Core"
end

group "Runtime" do
    include "Runtime"
end

