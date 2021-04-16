solution "edge-surf-game"
	location ( "build" )
	configurations { "Debug", "Release" }
	platforms {"x64", "x32"}
	warnings "Extra"
	defines { "NANOVG_GL3_IMPLEMENTATION", "GLEW_STATIC", "_CRT_SECURE_NO_WARNINGS" }

	project "nanovg"
		language "C"
		kind "StaticLib"
		includedirs { "nanovg/src" }
		files { "nanovg/src/*.c" }
		targetdir("build")

		configuration "Debug"
			defines { "DEBUG" }
			symbols "On"

		configuration "Release"
			defines { "NDEBUG" }
			optimize "On"

	project "Collections-C"
		language "C"
		kind "StaticLib"
		includedirs { "Collections-C/src/include" }
		files { "Collections-C/src/*.c" }
		targetdir("build")

		configuration "Debug"
			defines { "DEBUG" }
			symbols "On"

		configuration "Release"
			defines { "NDEBUG" }
			optimize "On"

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
			links { "glfw3" }
			linkoptions { "-framework OpenGL", "-framework Cocoa", "-framework IOKit", "-framework CoreVideo", "-framework Carbon" }

		configuration "Debug"
			defines { "DEBUG" }
			symbols "On"

		configuration "Release"
			defines { "NDEBUG" }
			optimize "On"
