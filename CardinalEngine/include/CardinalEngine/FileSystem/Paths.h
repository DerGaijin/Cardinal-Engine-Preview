#pragma once
#include "../Framework/Core/Headers.h"

namespace CE
{
	class CE_API Paths
	{
		friend class Engine;

	public:
		// Returns the Root Directory
		static std::filesystem::path RootDirectory();

		// Returns the Application Directory either the Project Application in Standalone builds or the Editor Application in Editor builds
		static std::filesystem::path ApplicationDirectory();

		// Returns the Path to the Engine Resources
		static const std::filesystem::path& EngineResources();

		// Returns the Project Work Directory
		static const std::filesystem::path& ProjectWorkDirectory();

		// Returns either the Project Working Directory or in Editor build the Editor Work Directory
		static const std::filesystem::path& WorkDirectory();

#if CE_EDITOR
		// Returns the Project Directory
		static const std::filesystem::path& ProjectDirectory();

		// Returns the Path to the Editor Resources
		static const std::filesystem::path& EditorResources();

		// Returns the Editor Working Directory
		static const std::filesystem::path& EditorWorkDirectory();
#endif

	private:
		static std::filesystem::path m_EngineResourcesPath;
		static std::filesystem::path m_ProjectWorkDirectoryPath;

#if CE_EDITOR
		static std::filesystem::path m_ProjectDirectoryPath;
		static std::filesystem::path m_EditorResourcesPath;
		static std::filesystem::path m_EditorWorkDirectoryPath;
#endif
	};
}
