#include "CardinalEngine/Game/Entity.h"
#include "CardinalEngine/Game/World.h"
#include "CardinalEngine/Game/Components/EntityComponent.h"
#include "CardinalEngine/Game/Components/TransformComponent.h"

namespace CE
{
	Entity::Entity()
	{
		SetRootComponent(SharedPtr<TransformComponent>(new TransformComponent()));
	}

	Entity::Entity(const SharedPtr<TransformComponent>& Root)
	{
		SetRootComponent(Root);
	}

	bool Entity::SetRootComponent(const SharedPtr<TransformComponent>& NewRoot)
	{
		if (NewRoot == nullptr || NewRoot->m_Entity != nullptr)
		{
			return false;
		}

		Array<SharedPtr<RenderProxy>> RenderProxies;
		SharedPtr<CE::World> EntityWorld = World();
		if (m_RootComponent != nullptr)
		{
			//@TODO: Copy Transform
			if (EntityWorld != nullptr)
			{
				World()->UnregisterComponents(m_RootComponent.Get(), RenderProxies);
			}
		}

		NewRoot->m_Entity = this;
		m_RootComponent = NewRoot;

#if CE_EDITOR
		if (m_RootComponent->EditorName.Size() == 0)
		{
			m_RootComponent->EditorName = "Root Component";
		}
#endif

		if (EntityWorld != nullptr)
		{
			EntityWorld->RegisterComponents(m_RootComponent, RenderProxies);
			EntityWorld->UpdateRenderProxies(RenderProxies, {});
		}

		return true;
	}

	bool Entity::AddComponent(const SharedPtr<EntityComponent>& Component)
	{
		if (Component == nullptr || Component->m_Entity != nullptr)
		{
			return false;
		}

		SharedPtr<TransformComponent> TransformComp = SharedPtr<TransformComponent>::DynamicCast(Component);
		if (TransformComp != nullptr)
		{
			return m_RootComponent->AddComponent(TransformComp);
		}
		else
		{
			SharedPtr<CE::World> EntityWorld = World();
			if (EntityWorld != nullptr)
			{
				Array<SharedPtr<RenderProxy>> RenderProxies;
				EntityWorld->RegisterComponents(Component, RenderProxies);
				EntityWorld->UpdateRenderProxies(RenderProxies, {});
			}
			m_Components.Add(Component);
			return true;
		}
	}

	bool Entity::RemoveComponent(EntityComponent* Component)
	{
		if (Component == nullptr || Component->m_Entity != this)
		{
			return false;
		}

		TransformComponent* TransformComp = dynamic_cast<TransformComponent*>(Component);
		if (TransformComp != nullptr)
		{
			return m_RootComponent->RemoveComponent(TransformComp);
		}
		else
		{
			SharedPtr<CE::World> EntityWorld = World();
			if (EntityWorld != nullptr)
			{
				Array<SharedPtr<RenderProxy>> RenderProxies;
				EntityWorld->UnregisterComponents(Component, RenderProxies);
				EntityWorld->UpdateRenderProxies({}, RenderProxies);
			}
			Component->m_Entity = nullptr;
			return m_Components.RemoveByPredicate([&](const SharedPtr<EntityComponent>& It) { return It.Get() == Component; }) > 0;
		}
	}

	bool Entity::RemoveComponent(const SharedPtr<EntityComponent>& Component)
	{
		return RemoveComponent(Component.Get());
	}
}
