#include "CardinalEngine-Editor/UI/Windows/MainEditor.h"
#include "CardinalEngine-Editor/EngineEditor/EditorGameInstance.h"

#include "CardinalEngine/Game/World.h"
#include "CardinalEngine/Game/Entity.h"
#include "CardinalEngine/Game/Components/EntityComponent.h"
#include "CardinalEngine/Game/Components/TransformComponent.h"
#include "CardinalEngine/Game/Components/CameraComponent.h"
#include "CardinalEngine/Game/Components/InputComponent.h"
#include "CardinalEngine-Editor/Game/EditorViewportEntity.h"

#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIImage.h"

namespace CE
{
	void MainEditor::Tick(float DeltaTime)
	{
		EditorGameInstance& EGI = *EditorGameInstance::Get();
		SharedPtr<World> SelectedWorld = EGI.SelectedWorld.Lock();

		if (Viewport.GetWorld() != SelectedWorld)
		{
			Viewport.SetWorld(SelectedWorld);
		}

		Viewport.Tick(DeltaTime);
	}

	void MainEditor::Draw(float DeltaTime)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Editor");
		ImGui::PopStyleVar();

		ImGuiWindowClass MainEditorWndClass;
		MainEditorWndClass.ClassId = ImGui::GetID("MainEdtorWindowClass");
		MainEditorWndClass.DockingAllowUnclassed = false;

		ImGui::DockSpace(MainEditorWndClass.ClassId, ImGui::GetContentRegionAvail(), 0, &MainEditorWndClass);

		ImGui::End();

		DrawViewport(&MainEditorWndClass);
		DrawWorldHierarchy(&MainEditorWndClass);
		DrawWorldSettings(&MainEditorWndClass);
		DrawEntityInspector(&MainEditorWndClass);
	}

	void MainEditor::DrawViewport(ImGuiWindowClass* WindowClass)
	{
		ImGui::SetNextWindowClass(WindowClass);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		ImGui::PopStyleVar();

		ImVec2 CursorPos = ImGui::GetCursorPos();
		ImVec2 Available = ImGui::GetContentRegionAvail();
		Viewport.ImGuiDraw({ Available.x, Available.y });
		ImGui::SetCursorPos(CursorPos);



		ImGui::End();
	}

	void MainEditor::DrawWorldHierarchy(ImGuiWindowClass* WindowClass)
	{
		ImGui::SetNextWindowClass(WindowClass);
		ImGui::Begin("World Hierarchy");

		EditorGameInstance& EGI = *EditorGameInstance::Get();
		SharedPtr<World> SelectedWorld = EGI.SelectedWorld.Lock();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::BeginCombo("##WorldSelection", SelectedWorld != nullptr ? SelectedWorld->EditorName.Data() : ""))
		{
			for (auto& World : EGI.Worlds())
			{
				bool Selected = World == SelectedWorld;
				if (ImGui::Selectable(String::Format("%s##%p", World->EditorName.Data(), World.Get()).Data(), &Selected))
				{
					SelectedWorld = World;
					EGI.SelectedWorld = SelectedWorld;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Separator();

		bool ShowEditorItems = true;	//@TODO: This is only Temporary
		if (SelectedWorld != nullptr)
		{
			if (ImGui::BeginTable("##EntityList", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH))
			{
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Class");
				ImGui::TableHeadersRow();

				const Array<SharedPtr<Entity>>& Entities = SelectedWorld->Entities();
				SharedPtr<Entity> SelectedEntity = SelectedWorld->SelectedEntity.Lock();
				for (auto& Entity : Entities)
				{
					if (ShowEditorItems || !Entity->IsEditorEntity)
					{
						if (Entity->IsEditorEntity)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
						}

						ImGui::TableNextRow();
						ImGui::TableNextColumn();

						bool IsSelected = Entity == SelectedEntity;
						if (ImGui::Selectable(String::Format("%s##%p", Entity->EditorName.Data(), Entity.Get()).Data(), &IsSelected, ImGuiSelectableFlags_SpanAllColumns))
						{
							if (IsSelected)
							{
								SelectedWorld->SelectedEntity = Entity;
							}
							else
							{
								SelectedWorld->SelectedEntity.Reset();
							}
						}

						ImGui::TableNextColumn();
						String ClassName = Entity->GetType().TypeName();
						ClassName = ClassName.SubString(ClassName.Find(' ') + 1);
						ImGui::Text(ClassName.Data());

						if (Entity->IsEditorEntity)
						{
							ImGui::PopStyleColor();
						}
					}
				}
				ImGui::EndTable();
			}
		}

		ImGui::End();
	}

	void MainEditor::DrawWorldSettings(ImGuiWindowClass* WindowClass)
	{
		ImGui::SetNextWindowClass(WindowClass);
		ImGui::Begin("World Settings");

		ImGui::End();
	}

	void MainEditor::DrawEntityInspector(ImGuiWindowClass* WindowClass)
	{
		ImGui::SetNextWindowClass(WindowClass);
		ImGui::Begin("Entity Inspector");

		EditorGameInstance& EGI = *EditorGameInstance::Get();
		SharedPtr<World> SelectedWorld = EGI.SelectedWorld.Lock();
		if (SelectedWorld != nullptr)
		{
			SharedPtr<Entity> SelectedEntity = SelectedWorld->SelectedEntity.Lock();
			if (SelectedEntity != nullptr)
			{
				bool IsEntitySelected = SelectedEntity->SelectedComponent.Expired();
				if (ImGui::Selectable(String::Format("%s##%p", SelectedEntity->EditorName.Data(), SelectedEntity.Get()).Data(), &IsEntitySelected))
				{
					SelectedEntity->SelectedComponent.Reset();
				}

				ImGui::Separator();

				DrawEntityInspector_CompSelect(SelectedEntity);

				ImGui::Separator();

				SharedPtr<Object> Obj = SelectedEntity;
				SharedPtr<EntityComponent> SelectedComponent = SelectedEntity->SelectedComponent.Lock();
				if (SelectedComponent != nullptr)
				{
					Obj = SelectedComponent;
				}

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				ImGui::InputText("##InputEditorName", &Obj->EditorName);

				if (SelectedComponent != nullptr)
				{
					SelectedComponent->DrawInspector();
				}
				else
				{
					SelectedEntity->DrawInspector();
				}
			}
		}

		ImGui::End();
	}

	void MainEditor::DrawEntityInspector_CompSelect(const SharedPtr<Entity>& SelectedEntity)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::BeginChild("##ComponentSelection", ImVec2(0, 150), true);
		ImGui::PopStyleVar();

		SharedPtr<EntityComponent> SelectedComp = SelectedEntity->SelectedComponent.Lock();
		DrawEntityInspector_CompHierarchy(SelectedEntity, SelectedEntity->GetRootComponent(), SelectedComp);

		if (SelectedEntity->GetComponents().Size() > 0)
		{
			ImGui::Separator();
			for (auto& Component : SelectedEntity->GetComponents())
			{
				bool IsComponentSelected = Component == SelectedComp;
				if (ImGui::Selectable(String::Format("%s##%p", Component->EditorName.Data(), Component.Get()).Data(), &IsComponentSelected))
				{
					if (IsComponentSelected)
					{
						SelectedEntity->SelectedComponent = Component;
					}
					else
					{
						SelectedEntity->SelectedComponent.Reset();
					}
				}
			}
		}
		ImGui::EndChild();
	}

	void MainEditor::DrawEntityInspector_CompHierarchy(const SharedPtr<Entity>& SelectedEntity, const SharedPtr<TransformComponent>& Component, const SharedPtr<EntityComponent>& SelectedComponent)
	{
		ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (Component == SelectedComponent)
		{
			Flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool IsTreeOpen = ImGui::TreeNodeEx(String::Format("%s##%p", Component->EditorName.Data(), Component.Get()).Data(), Flags);
		if (ImGui::IsItemActivated() && !ImGui::IsItemToggledOpen())
		{
			if (Component != SelectedComponent)
			{
				SelectedEntity->SelectedComponent = Component;
			}
			else
			{
				SelectedEntity->SelectedComponent.Reset();
			}
		}
		if (IsTreeOpen)
		{
			for (auto& Child : Component->GetChildren())
			{
				DrawEntityInspector_CompHierarchy(SelectedEntity, Child, SelectedComponent);
			}
			ImGui::TreePop();
		}
	}
}
