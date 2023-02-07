#include "CardinalEngine-Editor/UI/EditorUI.h"
#include "CardinalEngine/Engine/Engine.h"

#include "CardinalEngine-Editor/ImGuiInterface/Headers.h"

namespace CE
{
	EditorUI::EditorUI()
	{

	}

	void EditorUI::Tick(float DeltaTime)
	{
		AssetBrowser.Tick(DeltaTime);
		EditorLog.Tick(DeltaTime);
		MainEditor.Tick(DeltaTime);
		ProfilerDisplay.Tick(DeltaTime);
		StatisticsDisplay.Tick(DeltaTime);

		Array<SharedPtr<EditorWindow>> EditorWindows = Windows;
		for (auto& EW : EditorWindows)
		{
			if (!EW->IsOpen && EW->RemoveOnClosed())
			{
				Windows.Remove(EW);
			}

			EW->Tick(DeltaTime);
		}
	}

	void EditorUI::Draw(float DeltaTime)
	{
		DrawMainMenuBar();
		DrawDockSpace();

		if (m_ShowImGuiDemo)
		{
			ImGui::ShowDemoWindow(&m_ShowImGuiDemo);
		}

		if (AssetBrowser.IsOpen)
		{
			AssetBrowser.Draw(DeltaTime);
		}
		MainEditor.Draw(DeltaTime);
		if (ProfilerDisplay.IsOpen)
		{
			ProfilerDisplay.Draw(DeltaTime);
		}
		if (EditorLog.IsOpen)
		{
			EditorLog.Draw(DeltaTime);
		}
		if (StatisticsDisplay.IsOpen)
		{
			StatisticsDisplay.Draw(DeltaTime);
		}

		Array<SharedPtr<EditorWindow>> EditorWindows = Windows;
		for (auto& EW : EditorWindows)
		{
			if (EW->IsOpen)
			{
				EW->Draw(DeltaTime);
			}
		}
	}

	void EditorUI::DrawMainMenuBar()
	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("Engine"))
		{
			ImGui::MenuItem("Config");
			ImGui::Separator();
			if (ImGui::MenuItem("Restart"))
			{
				Engine::Get().RequestRestart();
			}
			if (ImGui::MenuItem("Shutdown"))
			{
				Engine::Get().RequestExit();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Project"))
		{
			ImGui::MenuItem("Config");
			ImGui::Separator();
			ImGui::MenuItem("Reload Scripts");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Asset Browser", nullptr, &AssetBrowser.IsOpen);
			ImGui::MenuItem("Editor Log", nullptr, &EditorLog.IsOpen);
			ImGui::MenuItem("Profiler", nullptr, &ProfilerDisplay.IsOpen);
			ImGui::MenuItem("Statistics", nullptr, &StatisticsDisplay.IsOpen);
			ImGui::Separator();
			ImGui::MenuItem("ImGui Demo", nullptr, &m_ShowImGuiDemo);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("Website");
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	void EditorUI::DrawDockSpace()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			window_flags |= ImGuiWindowFlags_NoBackground;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("CE_MainEditorDockspace", nullptr, window_flags);
			ImGui::PopStyleVar(3);

			ImGuiID dockspace_id = ImGui::GetID("CE_MainEditorDockspace");
			bool SetDocking = ImGui::DockBuilderGetNode(dockspace_id) == nullptr;
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, nullptr);

			if (SetDocking)
			{
				ImGuiID Dock_R = 0;
				ImGuiID Dock_L = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &Dock_R);

				ImGuiID Dock_RD = 0;
				ImGuiID Dock_RU = ImGui::DockBuilderSplitNode(Dock_R, ImGuiDir_Up, 0.7f, nullptr, &Dock_RD);
				
				ImGuiID Dock_RDR = 0;
				ImGuiID Dock_RDL = ImGui::DockBuilderSplitNode(Dock_RD, ImGuiDir_Left, 0.5f, nullptr, &Dock_RDR);

				ImGui::DockBuilderDockWindow("Statistics", Dock_L);
				ImGui::DockBuilderDockWindow("Editor", Dock_RU);
				ImGui::DockBuilderDockWindow("Asset Browser", Dock_RDL);
				ImGui::DockBuilderDockWindow("Profiler", Dock_RDR);
				ImGui::DockBuilderDockWindow("###EditorLog", Dock_RDR);
			}

			ImGui::End();
		}
	}
}
