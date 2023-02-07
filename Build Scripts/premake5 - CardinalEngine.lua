project "CardinalEngine"
	location "%{wks.location}/CardinalEngine"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir (BuildOutputDir)
	objdir (BuildIntOutputDir)
	debugdir "${wks.location}/"

	defines { "CE_BUILD" }
	disablewarnings { "4251", "4275", "4005", "4099", "26812" }

	files
	{
		"%{prj.location}/source/CardinalEngine/**.h",
		"%{prj.location}/source/CardinalEngine/**.cpp",
		"%{prj.location}/source/CardinalEngine/**.c",
		"%{prj.location}/include/CardinalEngine/**.h",
		"%{prj.location}/include/CardinalEngine/**.hpp",
		"%{prj.location}/Documents/**.natvis",
	}

	includedirs
	{
		"%{prj.location}/include/",
		"%{wks.location}/Thirdparty/include/"
	}

	filter { "configurations:Debug-Editor or configurations:Debug-Standalone" }
		links
		{
			LibraryPath_Debug .. "/GenericCodeGend.lib",
			LibraryPath_Debug .. "/glslangd.lib",
			LibraryPath_Debug .. "/glslang-default-resource-limitsd.lib",
			LibraryPath_Debug .. "/HLSLd.lib",
			LibraryPath_Debug .. "/MachineIndependentd.lib",
			LibraryPath_Debug .. "/OGLCompilerd.lib",
			LibraryPath_Debug .. "/OSDependentd.lib",
			LibraryPath_Debug .. "/SPIRVd.lib",
			LibraryPath_Debug .. "/SPVRemapperd.lib",
			LibraryPath_Debug .. "/spirv-cross-cored.lib",
			LibraryPath_Debug .. "/spirv-cross-hlsld.lib",
		}

	filter { "configurations:Release-Editor or configurations:Release-Standalone" }
		links
		{
			LibraryPath_Release .. "/GenericCodeGen.lib",
			LibraryPath_Release .. "/glslang.lib",
			LibraryPath_Release .. "/glslang-default-resource-limits.lib",
			LibraryPath_Release .. "/HLSL.lib",
			LibraryPath_Release .. "/MachineIndependent.lib",
			LibraryPath_Release .. "/OGLCompiler.lib",
			LibraryPath_Release .. "/OSDependent.lib",
			LibraryPath_Release .. "/SPIRV.lib",
			LibraryPath_Release .. "/SPVRemapper.lib",
			LibraryPath_Release .. "/spirv-cross-core.lib",
			LibraryPath_Release .. "/spirv-cross-hlsl.lib",
		}

	filter { "configurations:Debug-Editor or configurations:Debug-Standalone" }
		runtime "Debug"
		defines { "CE_DEBUG" }

	filter { "configurations:Release-Editor or configurations:Release-Standalone" }
		runtime "Release"

    filter { "configurations:Debug-Editor or configurations:Release-Editor" }
        defines { "CE_EDITOR" }
		files
		{
			"%{prj.location}/source/CardinalEngine-Editor/**.h",
			"%{prj.location}/source/CardinalEngine-Editor/**.cpp",
			"%{prj.location}/source/CardinalEngine-Editor/**.c",
			"%{prj.location}/include/CardinalEngine-Editor/**.h"
		}

	filter { "kind:SharedLib" }
		defines { "CE_DLL" }

