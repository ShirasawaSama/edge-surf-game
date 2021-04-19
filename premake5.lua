function builds()
	language "C"
	kind "StaticLib"
	targetdir("build")
	configuration "Debug"
		defines { "DEBUG" }
		symbols "On"
	configuration "Release"
		defines { "NDEBUG" }
		optimize "On"
end

solution "edge-surf-game"
	location ( "build" )
	configurations { "Debug", "Release" }
	platforms {"x64", "x32"}
	warnings "Extra"
	defines { "NANOVG_GL3_IMPLEMENTATION", "GLEW_STATIC", "_CRT_SECURE_NO_WARNINGS" }

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
			 linkoptions { "`pkg-config --libs glfw3`" }
			 links { "GL", "GLU", "m", "GLEW" }
			 defines { "NANOVG_GLEW" }

		configuration { "windows" }
			 links { "glfw3", "gdi32", "winmm", "user32", "GLEW", "glu32","opengl32", "kernel32" }
			 defines { "NANOVG_GLEW" }

		configuration { "macosx" }
			buildoptions { "`pkg-config --cflags glfw3`" }
			linkoptions { "-framework OpenGL", "-framework Cocoa", "-framework IOKit", "-framework CoreVideo", "`pkg-config --libs glfw3`" }

		configuration "Debug"
			defines { "DEBUG" }
			symbols "On"

		configuration "Release"
			defines { "NDEBUG" }
			optimize "On"
