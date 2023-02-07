#include "CardinalEngine/FileSystem/Paths.h"
#include "CardinalEngine/Framework/Platform/Platform.h"
#include "CardinalEngine/Framework/ModuleSystem/Module.h"

namespace CE
{
	std::filesystem::path Paths::m_EngineResourcesPath;
	std::filesystem::path Paths::m_ProjectWorkDirectoryPath;
#if CE_EDITOR
	std::filesystem::path Paths::m_ProjectDirectoryPath;
	std::filesystem::path Paths::m_EditorResourcesPath;
	std::filesystem::path Paths::m_EditorWorkDirectoryPath;
#endif

	std::filesystem::path Paths::RootDirectory()
	{
		return std::filesystem::current_path();
	}

	std::filesystem::path Paths::ApplicationDirectory()
	{
		return std::filesystem::absolute(Platform::Get().GetLibraryPath(nullptr).parent_path());
	}

	const std::filesystem::path& Paths::EngineResources()
	{
		return m_EngineResourcesPath;
	}

	const std::filesystem::path& Paths::ProjectWorkDirectory()
	{
		return m_ProjectWorkDirectoryPath;
	}

	const std::filesystem::path& Paths::WorkDirectory()
	{
#if CE_EDITOR
		return EditorWorkDirectory();
#else
		return ProjectWorkDirectory();
#endif
	}

#if CE_EDITOR
	const std::filesystem::path& Paths::ProjectDirectory()
	{
		return m_ProjectDirectoryPath;
	}

	const std::filesystem::path& Paths::EditorResources()
	{
		return m_EditorResourcesPath;
	}

	const std::filesystem::path& Paths::EditorWorkDirectory()
	{
		return m_EditorWorkDirectoryPath;
	}
#endif
}
