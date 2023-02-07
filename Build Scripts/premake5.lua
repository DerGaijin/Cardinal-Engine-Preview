workspace "CardinalEngine"
	location "../"
	architecture "x64"
	configurations { "Debug-Editor", "Debug-Standalone", "Release-Editor", "Release-Standalone" }
	startproject "DevelopmentProject"

BuildOutputDir = "%{wks.location}/Bin/%{cfg.system}/%{cfg.buildcfg} - %{cfg.architecture}"
BuildIntOutputDir = "%{wks.location}/Bin-int/%{cfg.system}/%{cfg.buildcfg} - %{cfg.architecture}"
LibraryPath_Debug = "%{wks.location}/Thirdparty/Libraries %{cfg.architecture} - Debug"
LibraryPath_Release = "%{wks.location}/Thirdparty/Libraries %{cfg.architecture} - Release"

group "Engine"
include "premake5 - CardinalEngine.lua"
include "premake5 - CardinalEngine-Editor.lua"
group ""

group "Projects"
include "premake5 - DevelopmentProject.lua"
group ""
