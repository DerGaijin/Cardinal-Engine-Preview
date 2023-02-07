project "DevelopmentProject"
	location "%{wks.location}/Projects/DevelopmentProject"

	filter { "configurations:Debug-Editor or configurations:Release-Editor" }
		kind "SharedLib"
	filter { "configurations:Debug-Standalone" }
		kind "ConsoleApp"
	filter { "configurations:Release-Standalone" }
		kind "WindowedApp"
	filter {}

	language "C++"
	cppdialect "C++17"

	targetdir (BuildOutputDir)
	objdir (BuildIntOutputDir)
	debugdir "%{wks.location}/"

	disablewarnings { "4251", "4275", "4005", "4099", "26812" }

	links
	{
		"CardinalEngine",
		"CardinalEngine-Editor"
	}

	files
	{
		"%{prj.location}/**.h",
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
		debugcommand "%{wks.location}/Bin/%{cfg.system}/%{cfg.buildcfg} - %{cfg.architecture}/CardinalEngine-Editor.exe"
		debugargs { "-Project=\"%{prj.location}/%{prj.name}.CEP\"", "-EngineResources=\"%{wks.location}/CardinalEngine - Resources\"" }

