#include "ProjectManagementGameInstance.h"
#include "CardinalEngine/Engine/Engine.h"

#include "CardinalEngine-Editor/EngineEditor/EngineEditor.h"
#include "CardinalEngine-Editor/EngineEditor/EditorGameInstance.h"
#include "CardinalEngine-Editor/ImGuiInterface/Headers.h"

namespace CEE
{
	bool ProjectManagementGameInstance::Initialize()
	{
		StartArgument StartProject = StartArgument::GetValue(L"Project");
		if (StartProject.IsValid())
		{
			std::filesystem::path ProjectPath = std::filesystem::absolute(StartProject.Text().Data()).lexically_normal();
			LoadAndSetProject(ProjectPath);
		}

		if (m_Project == nullptr)
		{
			m_ProjectSelectionWindow = SharedPtr<Window>(Engine::Get().Application()->CreateWindow(L"Project Management", Vec2ui(800, 500)));
			Engine::Get().DynamicRHI().CreateSwapchain(m_ProjectSelectionWindow.Get());

			EngineEditor::Get().ImGuiInterface.SetWindow(m_ProjectSelectionWindow);
		}

		return true;
	}

	void ProjectManagementGameInstance::BeginFrame()
	{
		SharedPtr<Swapchain> Swapchain = m_ProjectSelectionWindow->GetSwapchain();
		RenderThread::Enqueue([=](RHIContextImmediate& Context)
			{
				Context.PrepareSwapchain(Swapchain);

				SharedPtr<RHIImage> RT = Swapchain->CurrentImage();

				Context.BeginRenderpass({ RT }, nullptr);
				Context.ClearRenderTarget(RT, Color::FromRGB(0.1f, 0.1f, 0.1f));
				Context.EndRenderpass();
			});
	}

	void ProjectManagementGameInstance::Tick(float DeltaTime)
	{
		GameInstance::Tick(DeltaTime);

		if (m_ProjectSelectionWindow == nullptr || m_ProjectSelectionWindow->IsCloseRequested())
		{
			Engine::Get().RequestExit();
		}

		DrawUI();
	}

	void ProjectManagementGameInstance::EndFrame()
	{
		SharedPtr<Swapchain> Swapchain = m_ProjectSelectionWindow->GetSwapchain();
		RenderThread::Enqueue([=](RHIContextImmediate& Context)
			{
				Context.PresentSwapchain(Swapchain);
			});
	}

	void ProjectManagementGameInstance::Terminate()
	{
		m_ProjectSelectionWindow.Reset();
	}

	bool ProjectManagementGameInstance::LoadProjectFile(ProjectFileContent& Content, const std::filesystem::path& Path)
	{
		XmlDocument ProjectDocument;
		std::ifstream ProjectDocumentFile = std::ifstream(Path);
		if (!ProjectDocumentFile.is_open())
		{
			return false;
		}

		XmlDocumentParser Parser(ProjectDocument);
		Parser.SetSource(&ProjectDocumentFile);
		if (!Parser.Parse())
		{
			return false;
		}
		ProjectDocumentFile.close();

		if (ProjectDocument.HasElement("Project"))
		{
			const XmlElement& ProjectElement = ProjectDocument.GetElement("Project");

			if (ProjectElement.HasElement("Name"))
			{
				const XmlElement NameElement = ProjectElement.GetElement("Name");
				if (NameElement.SubElements().Size() == 1)
				{
					Content.Name = ProjectElement.GetElement("Name").SubElements()[0].Text();
				}
			}

			if (ProjectElement.HasElement("Binaries"))
			{
				const XmlElement& PathsElement = ProjectElement.GetElement("Binaries");
				if (PathsElement.HasElement("Debug"))
				{
					const XmlElement PathElement = PathsElement.GetElement("Debug");
					if (PathElement.SubElements().Size() == 1)
					{
						Content.Binaries.Debug = PathElement.SubElements()[0].Text().Data();
					}
				}
				if (PathsElement.HasElement("Release"))
				{
					const XmlElement PathElement = PathsElement.GetElement("Release");
					if (PathElement.SubElements().Size() == 1)
					{
						Content.Binaries.Release = PathElement.SubElements()[0].Text().Data();
					}
				}
			}
		}

		return true;
	}

	bool ProjectManagementGameInstance::SaveProjectFile(const ProjectFileContent& Content, const std::filesystem::path& Path)
	{
		XmlDocument ProjectDocument;
		ProjectDocument.AddElement(XmlElement(false, "xml", true)).SetAttribute("version", "1.0");
		XmlElement& ProjectElement = ProjectDocument.AddElement(XmlElement(false, "Project"));
		ProjectElement.AddSubElement(XmlElement(false, "Name")).AddSubElement(XmlElement(true, Content.Name));
		XmlElement& PathsElement = ProjectElement.AddSubElement(XmlElement(false, "Binaries"));
		PathsElement.AddSubElement(XmlElement(false, "Debug")).AddSubElement(XmlElement(true, Content.Binaries.Debug.u8string()));
		PathsElement.AddSubElement(XmlElement(false, "Release")).AddSubElement(XmlElement(true, Content.Binaries.Release.u8string()));

		std::ofstream ProjectDocumentFile = std::ofstream(Path);
		if (!ProjectDocumentFile.is_open())
		{
			return false;
		}
		String ProjectDocumentStr = ProjectDocument.Write();
		ProjectDocumentFile.write(ProjectDocumentStr.Data(), ProjectDocumentStr.Size());
		ProjectDocumentFile.close();
		return true;
	}

	bool ProjectManagementGameInstance::CreateProject(const ProjectCreationArguments& CreationArguments)
	{
		// Validate Arguments
		if (!ValidateCreationArguments(CreationArguments))
		{
			return false;
		}

		// Create Required Folders
		std::filesystem::create_directories(CreationArguments.Path / CreationArguments.Name.Data());

		std::filesystem::path ProjectFilePath = CreationArguments.Path / (CreationArguments.Name + ".CEP").Data();

		// Create ProjectFileContent
		ProjectFileContent Content;
		Content.Name = CreationArguments.Name;
		Content.Binaries.Debug = "/Bin/windows/Debug-Editor - x86_64/";
		Content.Binaries.Release = "/Bin/windows/Release-Editor - x86_64/";

		// Save Content to File
		if (!SaveProjectFile(Content, ProjectFilePath))
		{
			return false;
		}

		std::filesystem::path ProjectHeaderFilePath = CreationArguments.Path / CreationArguments.Name.Data() / (CreationArguments.Name + ".h").Data();
		std::filesystem::path ProjectSourceFilePath = CreationArguments.Path / CreationArguments.Name.Data() / (CreationArguments.Name + ".cpp").Data();

		// Generate Initial Header and Source File
		if (!std::filesystem::exists(ProjectHeaderFilePath) && !std::filesystem::exists(ProjectSourceFilePath))
		{
			String ProjectHeaderFileStr =
#include "Templates/ProjectHeaderFile.inl"
				;

			String ProjectSourceFileStr =
#include "Templates/ProjectSourceFile.inl"
				;

			// Replace all Placeholders
			ProjectHeaderFileStr.ReplaceAll("$PROJECT_NAME$", CreationArguments.Name);
			ProjectSourceFileStr.ReplaceAll("$PROJECT_NAME$", CreationArguments.Name);

			// Write Initial Header File
			std::ofstream ProjectHeaderFile = std::ofstream(ProjectHeaderFilePath);
			if (ProjectHeaderFile.is_open())
			{
				ProjectHeaderFile.write(ProjectHeaderFileStr.Data(), ProjectHeaderFileStr.Size());
				ProjectHeaderFile.close();
			}

			// Write Initial Source File
			std::ofstream ProjectSourceFile = std::ofstream(ProjectSourceFilePath);
			if (ProjectSourceFile.is_open())
			{
				ProjectSourceFile.write(ProjectSourceFileStr.Data(), ProjectSourceFileStr.Size());
				ProjectSourceFile.close();
			}
		}

		return true;
	}

	bool ProjectManagementGameInstance::GenerateProject(const ProjectFileContent& Content, const std::filesystem::path& Path, EProjectGenerationType GenerationType)
	{
		String ProjectPremake5FileStr =
#include "Templates/ProjectPremake5File.inl"
			;

		// Replace all Placeholders
		ProjectPremake5FileStr.ReplaceAll("$PROJECT_NAME$", Content.Name);
		String EngineDirectory = std::filesystem::absolute(Paths::ApplicationDirectory() / "../../../").lexically_normal().u8string();
		EngineDirectory.ReplaceAll("\\", "/");
		ProjectPremake5FileStr.ReplaceAll("$ENGINE_DIR$", EngineDirectory);

		std::filesystem::path ProjectPremake5FilePath = Path.parent_path() / "premake5.lua";

		// Write Premake5 File
		std::ofstream ProjectPremake5File = std::ofstream(ProjectPremake5FilePath);
		if (!ProjectPremake5File.is_open())
		{
			return false;
		}
		ProjectPremake5File.write(ProjectPremake5FileStr.Data(), ProjectPremake5FileStr.Size());
		ProjectPremake5File.close();

		// Select Premake5 Configuration
		Text Configuration = L"vs2022";
		switch (GenerationType)
		{
		default:
		case CEE::EProjectGenerationType::Premake5_vs2020:
			Configuration = L"vs2022";
			break;
		case CEE::EProjectGenerationType::Premake5_vs2019:
			Configuration = L"vs2019";
			break;
		}

		std::filesystem::path Premake5Path = Paths::RootDirectory() / "Build Scripts/premake5.exe";	//@TODO: Don't use Premake from the Build Scripts

		// Run Premake5
		Text Arguments = Text::Format(L"\"%s\" --file=\"%s\"", Configuration.Data(), ProjectPremake5FilePath.c_str());
		return Platform::Get().StartProcess(Premake5Path, Arguments);
	}

	Project* ProjectManagementGameInstance::LoadStartupProject(std::filesystem::path& ProjectPath)
	{
		StartArgument StartProject = StartArgument::GetValue(L"Project");
		if (StartProject.IsValid())
		{
			ProjectPath = std::filesystem::absolute(StartProject.Text().Data()).lexically_normal();
			return LoadProject(ProjectPath);
		}
		return nullptr;
	}

	Project* ProjectManagementGameInstance::LoadProject(const std::filesystem::path& Path)
	{
		ProjectFileContent Content;
		if (!LoadProjectFile(Content, Path))
		{
			return nullptr;
		}
		return LoadProject(Content, Path);
	}

	Project* ProjectManagementGameInstance::LoadProject(const ProjectFileContent& Content, const std::filesystem::path& Path)
	{
		// Load Shared Library
#if CE_DEBUG
		std::filesystem::path LibraryPath = Content.Binaries.Debug;
#else
		std::filesystem::path LibraryPath = Content.Binaries.Release;
#endif
		if (LibraryPath.is_relative())
		{
			std::filesystem::path Parent = Path.parent_path();
			LibraryPath = (Parent += LibraryPath).lexically_normal();
		}

		if (!std::filesystem::is_regular_file(LibraryPath))
		{
			LibraryPath /= Content.Name.Data();
		}

		void* LibraryPtr = Platform::Get().LoadDynamicLibrary(LibraryPath.c_str());
		if (LibraryPtr == nullptr)
		{
			return nullptr;
		}

		// Export Project
		typedef CE::Project* (*CE_Create_Project_t)();
		CE_Create_Project_t CE_Create_Project = (CE_Create_Project_t)Platform::Get().GetFunctionPointer(LibraryPtr, "CE_Create_Project");
		if (CE_Create_Project == nullptr)
		{
			Platform::Get().UnloadDynamicLibrary(LibraryPtr);
			return nullptr;
		}

		Project* LoadedProject = CE_Create_Project();
		if (LoadedProject == nullptr)
		{
			Platform::Get().UnloadDynamicLibrary(LibraryPtr);
			return nullptr;
		}

		return LoadedProject;
	}

	bool ProjectManagementGameInstance::ValidateCreationArguments(const ProjectCreationArguments& Arguments, String* Error /*= nullptr*/)
	{
		auto SetError = [&](const char* Err)
		{
			if (Error != nullptr)
			{
				*Error = Err;
			}
		};

		if (Arguments.Name.Size() < 4)
		{
			SetError("The Project Name has to be at least 4 Characters.");
			return false;
		}

		for (auto& C : Arguments.Name)
		{
			if ((C >= 'A' && C <= 'Z') || (C >= 'a' && C <= 'z') || C == '_')
			{
				continue;
			}
			else
			{
				SetError("The Name contains illegal Characters");
				return false;
			}
		}

		if (Arguments.Path.empty())
		{
			SetError("Project Path is empty");
			return false;
		}

		return true;
	}

	bool ProjectManagementGameInstance::LoadAndSetProject(const std::filesystem::path& Path)
	{
		m_Project = LoadProject(Path);
		if (m_Project != nullptr)
		{
			Engine::Get().RemoveGameInstance(this);

			SharedPtr<GameInstance> EGI = SharedPtr<EditorGameInstance>(new EditorGameInstance());

			CE_CHECK(Engine::Get().InitializeProject(m_Project, Path));

			Engine::Get().AddGameInstance(EGI);

			return true;
		}
		return false;
	}

	void ProjectManagementGameInstance::DrawUI()
	{
		Vec2ui WindowSize = m_ProjectSelectionWindow->GetSize();
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(WindowSize.X, WindowSize.Y));
		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		ImGui::Begin("Project Management", nullptr, ImGuiWindowFlags_NoDecoration);

		float Width = (ImGui::GetContentRegionAvail().x / 2.0f) - ImGui::GetStyle().FramePadding.x;
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(IsLoadTabOpen ? ImGuiCol_ButtonActive : ImGuiCol_Button));
		if (ImGui::Button("Load", ImVec2(Width, 0.0f)))
		{
			IsLoadTabOpen = true;
		}
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(!IsLoadTabOpen ? ImGuiCol_ButtonActive : ImGuiCol_Button));
		if (ImGui::Button("Create", ImVec2(Width, 0.0f)))
		{
			IsLoadTabOpen = false;
		}
		ImGui::PopStyleColor();
		ImGui::Separator();

		if (IsLoadTabOpen)
		{
			DrawLoadTab();
		}
		else
		{
			DrawCreateTab();
		}

		ImGui::End();
	}

	void ProjectManagementGameInstance::DrawLoadTab()
	{
		float ButtonHeight = ImGui::CalcTextSize("Open", NULL, true).y;
		ButtonHeight += (ImGui::GetStyle().WindowPadding.y);
		ButtonHeight += (ImGui::GetStyle().WindowBorderSize);
		ButtonHeight += (ImGui::GetStyle().ItemSpacing.y);

		ImGui::BeginChild("LoadTab", ImVec2(0.0f, ImGui::GetContentRegionAvail().y - ButtonHeight));

		float Width = ImGui::GetContentRegionAvail().x;

		for (size_t i = 0; i < 5; i++)
		{
			String ProjectName = String::Format("Project_%llu", i);
			std::filesystem::path ProjectPath = String::Format("Test/Path/For/Project_%llu.CEP", i).Data();

			String IDStr = ProjectPath.u8string();
			const char* ID = IDStr.Data();

			ImGui::PushID(ID);
			ImGui::BeginGroup();

			auto* DrawList = ImGui::GetWindowDrawList();

			float WrapWidth = Width - ImGui::GetStyle().ItemInnerSpacing.x;
			float TextHeight = ImGui::CalcTextSize(ProjectName.Data()).y;
			TextHeight += ImGui::CalcTextSize(ProjectPath.u8string().c_str(), 0, false, WrapWidth).y;
			TextHeight += ImGui::GetStyle().ItemSpacing.y;
			TextHeight += ImGui::GetStyle().ItemInnerSpacing.y * 2.0f;

			ImRect ItemRect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + Width, ImGui::GetCursorScreenPos().y + TextHeight));

			ImU32 BackgroundColor = IM_COL32(47, 47, 47, 255);
			if (ImGui::ItemHoverable(ItemRect, ImGui::GetID(ID)))
			{
				BackgroundColor = IM_COL32(60, 60, 60, 255);
			}
			DrawList->AddRectFilled(ItemRect.Min, ItemRect.Max, BackgroundColor);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemInnerSpacing.y);

			ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + WrapWidth);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemInnerSpacing.x);
			ImGui::Text(ProjectName.Data());
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemInnerSpacing.x);
			ImGui::Text(ProjectPath.u8string().c_str());
			ImGui::PopStyleColor();
			ImGui::PopTextWrapPos();

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::ItemHoverable(ItemRect, ImGui::GetID(ID)))
			{
				LoadAndSetProject(ProjectPath);
			}

			ImGui::EndGroup();
			ImGui::PopID();
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ItemRect.Max.y + (ImGui::GetStyle().ItemSpacing.y * 2)));
		}

		ImGui::EndChild();
		if (ImGui::Button("Open", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			Array<std::filesystem::path> ProjectPath;
			Platform::Get().ShowOpenFileDialog(ProjectPath, L"Open Project", { {L"Cardinal Engine Projects", L"*.CEP"} }, EFileDialogFlag::FileMustExist);
			if (ProjectPath.Size() == 1)
			{
				LoadAndSetProject(ProjectPath[0]);
			}
		}
	}

	void ProjectManagementGameInstance::DrawCreateTab()
	{
		float ButtonHeight = ImGui::CalcTextSize("Open", NULL, true).y;
		ButtonHeight += (ImGui::GetStyle().WindowPadding.y);
		ButtonHeight += (ImGui::GetStyle().WindowBorderSize);
		ButtonHeight += (ImGui::GetStyle().ItemSpacing.y);

		ImGui::BeginChild("CreateTab", ImVec2(0.0f, ImGui::GetContentRegionAvail().y - ButtonHeight));
		ImGui::Columns(2);

		ImGui::Text("Project Name");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText("##InputName", &CreationArguments.Name);


		ImGui::NextColumn();
		ImGui::Text("Project Path");
		ImGui::NextColumn();
		float ButtonWidth = ImGui::CalcTextSize("...", NULL, true).x;
		ButtonWidth = ImGui::CalcItemSize(ImVec2(0, 0), ButtonWidth + ImGui::GetStyle().FramePadding.x * 2.0f, 0.0f).x;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (ButtonWidth + ImGui::GetStyle().ItemSpacing.x));
		String Path = CreationArguments.Path.u8string();
		ImGui::InputText("##PathDisplay", &Path, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("..."))
		{
			Platform::Get().ShowBrowseFolderDialog(CreationArguments.Path, L"Select Project Folder");
		}

		ImGui::NextColumn();
		ImGui::Text("Generation Type");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::Combo("##InputGenerationType", (int*)&CreationArguments.GenerationType, "Premake5_vs2022\0Premake5_vs2019");

		ImGui::Columns(1);
		ImGui::EndChild();

		String Error;
		ValidateCreationArguments(CreationArguments, &Error);
		if (Error.Size() > 0)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
			ImGui::Text("( ! )");
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(Error.Data());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
		}
		ImGui::BeginDisabled(Error.Size() != 0);
		if (ImGui::Button("Confirm", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			if (CreateProject(CreationArguments))
			{
				std::filesystem::path ProjectFilePath = CreationArguments.Path / (CreationArguments.Name + ".CEP").Data();

				ProjectFileContent Content;
				if (LoadProjectFile(Content, ProjectFilePath))
				{
					if (GenerateProject(Content, ProjectFilePath, CreationArguments.GenerationType))
					{

					}
				}
			}
		}
		ImGui::EndDisabled();
	}
}
