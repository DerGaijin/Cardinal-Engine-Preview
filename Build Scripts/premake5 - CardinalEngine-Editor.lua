project "CardinalEngine-Editor"
	location "%{wks.location}/CardinalEngine-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir (BuildOutputDir)
	objdir (BuildIntOutputDir)
	debugdir "%{wks.location}"

	disablewarnings { "4251", "4275", "4005", "4099", "26812" }

	links { "CardinalEngine" }

	files
	{
		"%{prj.location}/**.h",
		"%{prj.location}/**.inl",
		"%{prj.location}/**.cpp",
	}

	includedirs
	{
		"%{wks.location}/CardinalEngine/include/"
	}

	filter { "configurations:Debug-Editor or configurations:Debug-Standalone" }
		runtime "Debug"
		defines { "CE_DEBUG" }

	filter { "configurations:Release-Editor or configurations:Release-Standalone" }
		runtime "Release"

    filter { "configurations:Debug-Editor or configurations:Release-Editor" }
        defines { "CE_EDITOR" }