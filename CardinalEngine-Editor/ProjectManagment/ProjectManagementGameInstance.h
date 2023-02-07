#pragma once
#include "CardinalEngine/CardinalEngine.h"

namespace CEE
{
	enum class EProjectGenerationType
	{
		Premake5_vs2020,
		Premake5_vs2019,
	};

	struct ProjectCreationArguments
	{
		String Name;
		std::filesystem::path Path;
		EProjectGenerationType GenerationType = EProjectGenerationType::Premake5_vs2020;
	};

	struct ProjectFileContent
	{
		String Name;
		struct
		{
			std::filesystem::path Debug;
			std::filesystem::path Release;
		} Binaries;
	};

	class ProjectManagementGameInstance : public GameInstance
	{
	public:
		ProjectManagementGameInstance(Project*& InProject) : m_Project(InProject) {}

		virtual bool Initialize() override;

		virtual void BeginFrame() override;

		virtual void Tick(float DeltaTime) override;

		virtual void EndFrame() override;

		virtual void Terminate() override;

	public:
		static bool LoadProjectFile(ProjectFileContent& Content, const std::filesystem::path& Path);

		static bool SaveProjectFile(const ProjectFileContent& Content, const std::filesystem::path& Path);

		static bool CreateProject(const ProjectCreationArguments& Arguments);

		static bool GenerateProject(const ProjectFileContent& Content, const std::filesystem::path& Path, EProjectGenerationType GenerationType);

		static Project* LoadStartupProject(std::filesystem::path& ProjectPath);

		static Project* LoadProject(const std::filesystem::path& Path);

		static Project* LoadProject(const ProjectFileContent& Content, const std::filesystem::path& Path);

		static bool ValidateCreationArguments(const ProjectCreationArguments& Arguments, String* Error = nullptr);

	private:
		bool LoadAndSetProject(const std::filesystem::path& Path);

		void DrawUI();
		void DrawLoadTab();
		void DrawCreateTab();

	public:
		bool IsLoadTabOpen = true;
		ProjectCreationArguments CreationArguments;

	private:
		Project*& m_Project;

		SharedPtr<Window> m_ProjectSelectionWindow;

	};
}
