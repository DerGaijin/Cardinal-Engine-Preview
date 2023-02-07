#include "CardinalEngine-Editor/UI/EditorViewport.h"
#include "CardinalEngine-Editor/ImGuiInterface/Headers.h"
#include "CardinalEngine-Editor/EngineEditor/EditorGameInstance.h"
#include "CardinalEngine/Application/Window.h"

#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/RHI/RHIImage.h"

#include "CardinalEngine/Game/World.h"
#include "CardinalEngine/Game/Entity.h"
#include "CardinalEngine/Game/Components/EntityComponent.h"
#include "CardinalEngine/Game/Components/TransformComponent.h"
#include "CardinalEngine/Game/Components/CameraComponent.h"
#include "CardinalEngine/Game/Components/InputComponent.h"
#include "CardinalEngine-Editor/Game/EditorViewportEntity.h"

namespace CE
{
	EditorViewport::EditorViewport()
	{
		m_Entity = SharedPtr<EditorViewportEntity>(new EditorViewportEntity());
		m_Entity->Input()->SetEnabled(false);
	}

	void EditorViewport::Tick(float DeltaTime)
	{
		SharedPtr<World> SelectedWorld = m_World.Lock();
		if (SelectedWorld == nullptr)
		{
			if (m_ViewportInput)
			{
				m_Entity->Input()->SetEnabled(false);
				EditorGameInstance::Get()->EditorWindow()->SetCursorMode(ECursorMode::Default);
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
				m_ViewportInput = false;
			}

			return;
		}

		if (m_Entity->World() != SelectedWorld)
		{
			SharedPtr<World> EntityWorld = m_Entity->World();
			if (EntityWorld != nullptr)
			{
				EntityWorld->RemoveEntity(m_Entity);
			}

			SelectedWorld->AddEntity(m_Entity);
		}

		if (m_RenderTarget == nullptr || !m_RenderTarget->GetTargetImage()->IsValid() || m_RenderTarget->GetTargetImage()->Description.Width != m_RenderTargetWidth || m_RenderTarget->GetTargetImage()->Description.Height != m_RenderTargetHeight)
		{
			if (m_RenderTargetWidth > 0 && m_RenderTargetHeight > 0)
			{
				RHIImageDescription RenderTargetDesc;
				RenderTargetDesc.Usage = ERHIImageUsage::RenderTarget | ERHIImageUsage::ShaderResource;
				RenderTargetDesc.Width = m_RenderTargetWidth;
				RenderTargetDesc.Height = m_RenderTargetHeight;
				SharedPtr<RHIImage> TargetImage = Engine::Get().DynamicRHI().CreateImage(RenderTargetDesc);

				m_RenderTarget = SharedPtr<ImageRenderTarget>(new ImageRenderTarget(TargetImage));

				m_Entity->Camera()->SetRenderTarget(m_RenderTarget);
			}
		}

		if (m_Entity->Camera()->GetRenderTarget() != m_RenderTarget)
		{
			m_Entity->Camera()->SetRenderTarget(m_RenderTarget);
		}

		if (m_ViewportInput && !Engine::Get().Application()->IsInputPressed(EInput::Mouse_Right))
		{
			m_Entity->Input()->SetEnabled(false);
			EditorGameInstance::Get()->EditorWindow()->SetCursorMode(ECursorMode::Default);
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			m_ViewportInput = false;
		}
	}

	void EditorViewport::ImGuiDraw(const Vec2f& Size)
	{
		m_RenderTargetWidth = Size.X;
		m_RenderTargetHeight = Size.Y;
		if (m_RenderTarget != nullptr)
		{
			ImGui::Image(m_RenderTarget->GetTargetImage(), ImVec2(m_RenderTargetWidth, m_RenderTargetHeight));
			ImVec2 RectMin = ImGui::GetItemRectMin();
			ImVec2 RectMax = ImGui::GetItemRectMax();
			if (ImGui::IsMouseHoveringRect(RectMin, RectMax) && ImGui::IsKeyDown(ImGuiKey_MouseRight) && !m_ViewportInput && m_Entity != nullptr)
			{
				m_Entity->Input()->SetEnabled(true);
				EditorGameInstance::Get()->EditorWindow()->SetCursorMode(ECursorMode::Disabled);
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
				m_ViewportInput = true;
			}
		}
		else
		{
			ImGui::BeginDisabled(true);
			ImGui::Button("VIEWPORT HAS NO WORLD", ImVec2(Size.X, Size.Y));
			ImGui::EndDisabled();
		}
	}

	void EditorViewport::SetWorld(const SharedPtr<World>& World)
	{
		m_World = World;
	}
}
