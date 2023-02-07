#include "CardinalEngine/Game/Components/TransformComponent.h"
#include "CardinalEngine/Game/Entity.h"
#include "CardinalEngine/Game/World.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Game/RenderProxies/SceneRenderProxy.h"

#if CE_EDITOR
#include "CardinalEngine-Editor/ImGuiInterface/Headers.h"
#endif

namespace CE
{
	bool TransformComponent::AddComponent(const SharedPtr<TransformComponent>& Component)
	{
		if (Component == nullptr || Component.Get() == this || !Component->m_Parent.Expired() || Component->Entity() != nullptr)
		{
			return false;
		}

		Component->m_Parent = SharedPtr<TransformComponent>::DynamicCast(GetShared());
		m_Children.Emplace(Component);

		SharedPtr<CE::Entity> OwnerEntity = Entity();
		if (OwnerEntity != nullptr)
		{
			SharedPtr<World> EntityWorld = OwnerEntity->World();
			if (EntityWorld != nullptr)
			{
				Array<SharedPtr<RenderProxy>> RenderProxies;
				EntityWorld->RegisterComponents(Component, RenderProxies);
				EntityWorld->UpdateRenderProxies(RenderProxies, {});
			}
		}

		Component->UpdateWorldTransform();

		return true;
	}

	bool TransformComponent::RemoveComponent(TransformComponent* Component)
	{
		if (Component == nullptr || Component->GetParent().Get() != this)
		{
			return false;
		}

		SharedPtr<CE::Entity> OwnerEntity = Entity();
		if (OwnerEntity != nullptr)
		{
			SharedPtr<World> EntityWorld = OwnerEntity->World();
			if (EntityWorld != nullptr)
			{
				Array<SharedPtr<RenderProxy>> RenderProxies;
				EntityWorld->UnregisterComponents(Component, RenderProxies);
				EntityWorld->UpdateRenderProxies({}, RenderProxies);
			}
		}

		Component->m_Parent.Reset();
		m_Children.RemoveByPredicate([&](const SharedPtr<TransformComponent>& It) { return It.Get() == Component; });

		return true;
	}

	bool TransformComponent::RemoveComponent(const SharedPtr<TransformComponent>& Component)
	{
		return RemoveComponent(Component.Get());
	}

	SharedPtr<TransformComponent> TransformComponent::GetParent() const
	{
		return m_Parent.Lock();
	}

	SharedPtr<Entity> TransformComponent::Entity() const
	{
		SharedPtr<TransformComponent> Parent = GetParent();
		return Parent != nullptr ? Parent->Entity() : EntityComponent::Entity();
	}

	Vec3f TransformComponent::GetForwardVector() const
	{
		Quaternion Quat = Quaternion::FromEuler(Math::Radians(m_WorldTransform.Rotation));
		return Quat.Rotate(World::ForwardVector);
	}

	Vec3f TransformComponent::GetUpVector() const
	{
		Quaternion Quat = Quaternion::FromEuler(Math::Radians(m_WorldTransform.Rotation));
		return Quat.Rotate(World::UpVector);
	}

	Vec3f TransformComponent::GetRightVector() const
	{
		Quaternion Quat = Quaternion::FromEuler(Math::Radians(m_WorldTransform.Rotation));
		return Quat.Rotate(World::RightVector);
	}

	void TransformComponent::SetRelativeLocation(const Vec3f& NewLocation)
	{
		m_RelativeTransform.Location = NewLocation;
		UpdateWorldTransform();
		UpdateChildrenTransform();
	}

	void TransformComponent::SetRelativeRotation(const Vec3f& NewRotation)
	{
		m_RelativeTransform.Rotation = NewRotation;
		UpdateWorldTransform();
		UpdateChildrenTransform();
	}

	void TransformComponent::SetRelativeScale(const Vec3f& NewScale)
	{
		m_RelativeTransform.Scale = NewScale;
		UpdateWorldTransform();
		UpdateChildrenTransform();
	}

	void TransformComponent::UpdateChildrenTransform()
	{
		for (auto& Child : m_Children)
		{
			Child->UpdateWorldTransform();
			Child->UpdateChildrenTransform();
		}
	}

	void TransformComponent::UpdateWorldTransform()
	{
		SharedPtr<TransformComponent> Parent = m_Parent.Lock();
		if (Parent != nullptr)
		{
			m_WorldTransform = m_RelativeTransform * Parent->m_WorldTransform;
		}
		else
		{
			m_WorldTransform = m_RelativeTransform;
		}
	}

#if CE_EDITOR
	void TransformComponent::DrawInspector()
	{
		EntityComponent::DrawInspector();
		if (ImGui::CollapsingHeader("Transform Component", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			bool Changed = false;
			auto DrawVec = [&](const char* ID, Vec3f& Val, const Vec3f& Default, float Speed)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

				// Calculate Width
				ImGuiStyle& Style = ImGui::GetStyle();
				const ImVec2 label_size = ImGui::CalcTextSize("X", NULL, true);
				float ButtonSize = ImGui::CalcItemSize(ImVec2(0.0f, 0.0f), label_size.x + Style.FramePadding.x * 2.0f, label_size.y + Style.FramePadding.y * 2.0f).y;
				float DragWidth = (ImGui::GetContentRegionAvail().x / 3.0f) - ButtonSize;

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				if (ImGui::Button(String::Format("X##%s_X", ID).Data(), ImVec2(ButtonSize, ButtonSize)))
				{
					Val.X = Default.X;
					Changed = true;
				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(DragWidth);
				if (ImGui::DragFloat(String::Format("##%s_X", ID).Data(), &Val.X, Speed))
				{
					Changed = true;
				}

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				if (ImGui::Button(String::Format("Y##%s_Y", ID).Data(), ImVec2(ButtonSize, ButtonSize)))
				{
					Val.Y = Default.Y;
					Changed = true;
				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(DragWidth);
				if (ImGui::DragFloat(String::Format("##%s_Y", ID).Data(), &Val.Y, Speed))
				{
					Changed = true;
				}

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				if (ImGui::Button(String::Format("Z##%s_Z", ID).Data(), ImVec2(ButtonSize, ButtonSize)))
				{
					Val.Z = Default.Z;
					Changed = true;
				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(DragWidth);
				if (ImGui::DragFloat(String::Format("##%s_Z", ID).Data(), &Val.Z, Speed))
				{
					Changed = true;
				}

				ImGui::PopStyleVar();
			};

			ImGui::Columns(2);
			ImGui::Text("Location");
			ImGui::NextColumn();
			DrawVec("Location", m_RelativeTransform.Location, Vec3f(0.0f), 1.0f);
			ImGui::NextColumn();
			ImGui::Text("Rotation");
			ImGui::NextColumn();
			DrawVec("Rotation", m_RelativeTransform.Rotation, Vec3f(0.0f), 1.0f);
			ImGui::NextColumn();
			ImGui::Text("Scale");
			ImGui::NextColumn();
			DrawVec("Scale", m_RelativeTransform.Scale, Vec3f(1.0f), 0.01f);
			ImGui::Columns(1);

			if (Changed)
			{
				UpdateWorldTransform();
				UpdateChildrenTransform();
			}
		}
	}
#endif
}
