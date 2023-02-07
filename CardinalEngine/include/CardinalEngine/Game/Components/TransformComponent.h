#pragma once
#include "../../Framework/Core/Headers.h"
#include "EntityComponent.h"

namespace CE
{
	class RenderProxy;

	class CE_API TransformComponent : public EntityComponent
	{
		CE_OBJECT_BODY();
	private:
		friend class World;

	public:
		bool AddComponent(const SharedPtr<TransformComponent>& Component);

		bool RemoveComponent(TransformComponent* Component);

		bool RemoveComponent(const SharedPtr<TransformComponent>& Component);

	protected:
		virtual SharedPtr<RenderProxy> GetRenderProxy() const
		{
			return nullptr;
		}

	public:
		void SetRelativeLocation(const Vec3f& NewLocation);

		void SetRelativeRotation(const Vec3f& NewRotation);

		void SetRelativeScale(const Vec3f& NewScale);

	public:
		SharedPtr<TransformComponent> GetParent() const;

		virtual SharedPtr<CE::Entity> Entity() const override;

		const Array<SharedPtr<TransformComponent>>& GetChildren() const
		{
			return m_Children;
		}

		const Vec3f& GetRelativeLocation() const
		{
			return m_RelativeTransform.Location;
		}

		const Vec3f& GetRelativeRotation() const
		{
			return m_RelativeTransform.Rotation;
		}

		const Vec3f& GetRelativeScale() const
		{
			return m_RelativeTransform.Scale;
		}

		const Transform& GetRelativeTransform() const
		{
			return m_RelativeTransform;
		}

		const Vec3f& GetWorldLocation() const
		{
			return m_WorldTransform.Location;
		}

		const Vec3f& GetWorldRotation() const
		{
			return m_WorldTransform.Rotation;
		}

		const Vec3f& GetWorldScale() const
		{
			return m_WorldTransform.Scale;
		}

		const Transform& GetWorldTransform() const
		{
			return m_WorldTransform;
		}

		Vec3f GetForwardVector() const;

		Vec3f GetUpVector() const;

		Vec3f GetRightVector() const;

	protected:
		virtual void UpdateWorldTransform();

#if CE_EDITOR
	protected:
		virtual void DrawInspector() override;
#endif

	private:
		void UpdateChildrenTransform();

	private:
		friend class CameraComponent;
		WeakPtr<TransformComponent> m_Parent;
		Array<SharedPtr<TransformComponent>> m_Children;

		Transform m_RelativeTransform;
		Transform m_WorldTransform;
	};
}
