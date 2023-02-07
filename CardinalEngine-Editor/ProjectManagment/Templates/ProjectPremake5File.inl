R"-(workspace "$PROJECT_NAME$"
	location "."
	architecture "x64"
	configurations { "Debug-Editor", "Debug-Standalone", "Release-Editor", "Release-Standalone" }
	startproject "$PROJECT_NAME$"

BuildOutputDir = "%{wks.location}/Bin/%{cfg.system}/%{cfg.buildcfg} - %{cfg.architecture}"
BuildIntOutputDir = "%{wks.location}/Bin-int/%{cfg.system}/%{cfg.buildcfg} - %{cfg.architecture}"

project "$PROJECT_NAME$"
	location "%{wks.location}/$PROJECT_NAME$"

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
		"$ENGINE_DIR$/Bin/%{cfg.system}/%{cfg.buildcfg} - %{cfg.architecture}/CardinalEngine.lib"
	}

	files
	{
		"%{prj.location}/**.h",
		"%{prj.location}/**.cpp",
	}

	includedirs
	{
		"$ENGINE_DIR$/CardinalEngine/include/"
	}

	filter { "configurations:Debug-Editor or configurations:Debug-Standalone" }
		runtime "Debug"
		defines { "CE_DEBUG" }

	filter { "configurations:Release-Editor or configurations:Release-Standalone" }
		runtime "Release"

	filter { "configurations:Debug-Editor or configurations:Release-Editor" }
		defines { "CE_EDITOR" }
		debugcommand "$ENGINE_DIR$/Bin/%{cfg.system}/%{cfg.buildcfg} - %{cfg.architecture}/CardinalEngine-Editor.exe"
		debugargs { "-Project=\"$PROJECT_NAME$.CEP\"", "-EngineResources=\"$ENGINE_DIR$/CardinalEngine - Resources\"" }

	filter { "configurations:Debug-Standalone or configurations:Release-Standalone" }
		postbuildcommands 
		{
			'{COPY} "$ENGINE_DIR$/Bin/%{cfg.system}/%{cfg.buildcfg} - %{cfg.architecture}/CardinalEngine.dll" "%{wks.location}/Bin/%{cfg.system}/%{cfg.buildcfg} - %{cfg.architecture}/"'
		}
)-"
