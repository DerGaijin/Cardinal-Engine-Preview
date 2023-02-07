#include "CardinalEngine/Game/Components/CameraComponent.h"
#include "CardinalEngine/Game/RenderProxies/RP_Camera.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Rendering/RenderTarget.h"
#include "CardinalEngine/Rendering/RHI/RHIImage.h"

#if CE_EDITOR
#include "CardinalEngine-Editor/ImGuiInterface/Headers.h"
#endif

namespace CE
{
	CameraComponent::CameraComponent()
	{
		m_RenderProxy = SharedPtr<RP_Camera>(new RP_Camera());

		UpdateWorldTransform();
	}

	void CameraComponent::SetRenderTarget(const SharedPtr<RenderTarget>& RenderTarget)
	{
		m_RenderTarget = RenderTarget;

		RP_Camera::ProjViewBuffData ProjectionData;
		CalculateProjection(ProjectionData);

		SharedPtr<RP_Camera> Proxy = m_RenderProxy;
		SharedPtr<CE::RenderTarget> NewRenderTarget = RenderTarget;
		RenderThread::Enqueue([=](RHIContextImmediate& Context)
			{
				m_RenderProxy->SetRenderTarget(NewRenderTarget);
				m_RenderProxy->SetProjectionData(Context, ProjectionData);
			});
	}

	void CameraComponent::CalculateProjection(RP_Camera::ProjViewBuffData& Data)
	{
		RHIImageDescription ImageDesc;
		ImageDesc.Width = 1920.0f;
		ImageDesc.Height = 1080.0f;
		if (m_RenderTarget != nullptr)
		{
			ImageDesc = m_RenderTarget->GetTargetImage()->Description;
		}

		if (CameraMode == ECameraMode::Perspective)
		{
			Data.Projection = Mat4x4f::PerspectiveFovRH_ZO(Math::Radians(FOV), ImageDesc.Width, ImageDesc.Height, ZNear, ZFar);
		}
		else
		{
			float aspect = ImageDesc.Width / ImageDesc.Height;
			Data.Projection = Mat4x4f::OrthoRH_ZO(-aspect, aspect, -1.0f, 1.0f, ZNear, ZFar);
		}

		Data.View = Mat4x4f::LookAtRH(GetWorldTransform().Location, GetWorldTransform().Location + GetForwardVector(), GetUpVector());
		Data.ViewPos = GetWorldTransform().Location;

		Data.ProjectionView = Data.Projection * Data.View;
	}

	void CameraComponent::UpdateWorldTransform()
	{
		TransformComponent::UpdateWorldTransform();

		RP_Camera::ProjViewBuffData ProjectionData;
		CalculateProjection(ProjectionData);

		SharedPtr<RP_Camera> CameraProxy = m_RenderProxy;
		RenderThread::Enqueue([=](RHIContextImmediate& Context)
			{
				CameraProxy->SetProjectionData(Context, ProjectionData);
			});
	}

	SharedPtr<RenderProxy> CameraComponent::GetRenderProxy() const
	{
		return m_RenderProxy;
	}

#if CE_EDITOR
	void CameraComponent::DrawInspector()
	{
		TransformComponent::DrawInspector();

		if (ImGui::CollapsingHeader("Camera Component", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			bool Changed = false;

			ImGui::Columns(2);

			ImGui::Text("Type");
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::Combo("##Type", (int*)&CameraMode, "Perspective\0Orthographic"))
			{
				Changed = true;
			}

			ImGui::NextColumn();
			ImGui::Text("FOV");
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::DragFloat("##FOV", &FOV, 1.0f, 1.0f))
			{
				Changed = true;
			}
			if (FOV <= 0.0f)
			{
				FOV = 1.0f;
			}

			ImGui::NextColumn();
			ImGui::Text("ZNear");
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::DragFloat("##ZNear", &ZNear))
			{
				Changed = true;
			}

			ImGui::NextColumn();
			ImGui::Text("ZFar");
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::DragFloat("##ZFar", &ZFar))
			{
				Changed = true;
			}

			ImGui::Columns(1);

			if (Changed)
			{
				UpdateWorldTransform();
				//UpdateProjection();
			}
		}
	}
#endif
}
