require('vstudio')

function vcpkg()
    premake.w("<VcpkgTriplet Condition=\"'$(Platform)'=='Win32'\">x86-windows-static</VcpkgTriplet>")
    premake.w("<VcpkgTriplet Condition=\"'$(Platform)'=='x64'\">x64-windows-static</VcpkgTriplet>")
    premake.w("<VcpkgEnabled>true</VcpkgEnabled>")
end

premake.override(premake.vstudio.vc2010.elements, "globals", function(base, prj)
    local calls = base(prj)
    table.insertafter(calls, premake.vstudio.vc2010.globals, vcpkg)
    return calls
end)

function builds()
    language "C"
    kind "StaticLib"
    targetdir("build")
end

solution "edge-surf-game"
    location ( "build" )
    configurations { "Debug", "Release" }
    platforms {"x64", "x32"}
    warnings "Extra"
    defines { "NANOVG_GL3_IMPLEMENTATION", "GLEW_STATIC", "_CRT_SECURE_NO_WARNINGS", "MINIAUDIO_IMPLEMENTATION" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    project "nanovg"
        includedirs { "nanovg/src" }
        files { "nanovg/src/*.c" }
        builds()

    project "Collections-C"
        includedirs { "Collections-C/src/include" }
        files { "Collections-C/src/*.c" }
        builds()

    project "edge-surf-game"
        kind "ConsoleApp"
        language "C"
        files { "src/*.c" }
        includedirs { "src", "nanovg/src", "Collections-C/src/include", "miniaudio" }
        targetdir("build")
        links { "nanovg", "Collections-C" }

        configuration { "linux" }
            linkoptions { "`pkg-config --libs glfw3`", "-ldl", "-pthread" }
            links { "GL", "GLU", "m", "GLEW" }
            defines { "NANOVG_GLEW" }

        configuration { "windows" }
            links { "glfw3", "gdi32", "winmm", "user32", "glu32", "opengl32", "kernel32" }
            defines { "NANOVG_GLEW" }

        configuration { "macosx" }
            defines { "GLFW_INCLUDE_GLCOREARB" }
            buildoptions { "`pkg-config --cflags glfw3`" }
            linkoptions { "-framework OpenGL", "-framework Cocoa", "-framework IOKit", "-framework CoreVideo", "`pkg-config --libs glfw3`" }

        filter { "action:vs*", "Debug" }
            links { "glew32d" }

        filter { "action:vs*", "Release" }
            links { "glew32" }
