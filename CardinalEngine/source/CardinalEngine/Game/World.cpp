#include "CardinalEngine/Game/World.h"
#include "CardinalEngine/Game/RenderProxies/SceneRenderProxy.h"
#include "CardinalEngine/Game/Entity.h"
#include "CardinalEngine/Game/Components/EntityComponent.h"
#include "CardinalEngine/Game/Components/TransformComponent.h"
#include "CardinalEngine/Engine/Engine.h"

namespace CE
{
	World::World()
	{
		m_SceneRenderProxy = SharedPtr<SceneRenderProxy>(new SceneRenderProxy());
	}

	void World::Tick(float DeltaTime)
	{
		for (auto& Entity : m_Entities)
		{
			Entity->Tick(DeltaTime);
		}
	}

	bool World::AddEntity(const SharedPtr<Entity>& Entity)
	{
		if (Entity->m_World.Lock() != nullptr)
		{
			return false;
		}

		m_Entities.Emplace(Entity);
		Entity->m_World = SharedPtr<World>::DynamicCast(GetShared());

		Array<SharedPtr<RenderProxy>> RenderProxies;
		RegisterComponents(Entity->GetRootComponent(), RenderProxies);
		for (auto& Comp : Entity->GetComponents())
		{
			RegisterComponents(Comp, RenderProxies);
		}

		UpdateRenderProxies(RenderProxies, {});

		return true;
	}

	bool World::RemoveEntity(const SharedPtr<Entity>& Entity)
	{
		return RemoveEntity(Entity.Get());
	}

	bool World::RemoveEntity(Entity* Entity)
	{
		if (Entity->m_World.Lock().Get() != this)
		{
			return false;
		}

		m_Entities.RemoveByPredicate([&](const SharedPtr<CE::Entity>& It) { return It.Get() == Entity; });
		Entity->m_World.Reset();

		Array<SharedPtr<RenderProxy>> RenderProxies;
		for (auto& Comp : Entity->GetComponents())
		{
			UnregisterComponents(Comp.Get(), RenderProxies);
		}
		UnregisterComponents(Entity->GetRootComponent().Get(), RenderProxies);

		UpdateRenderProxies({}, RenderProxies);

		return true;
	}

	void World::IterateComponents(const Type& Type, const FunctionRef<bool(const SharedPtr<EntityComponent>&)>& Iter, bool UseInherited /*= true*/) const
	{
		if (UseInherited)
		{
			for (auto& CompType : m_Components)
			{
				if (CompType.first == Type || CompType.first.IsChildOf(Type))
				{
					for (auto& Component : CompType.second)
					{
						if (!Iter(Component))
						{
							return;
						}
					}
				}
			}
		}
		else
		{
			auto Found = m_Components.find(Type);
			if (Found != m_Components.end())
			{
				for (auto& Component : Found->second)
				{
					if (!Iter(Component))
					{
						return;
					}
				}
			}
		}
	}

	void World::RegisterComponents(const SharedPtr<EntityComponent>& Component, Array<SharedPtr<RenderProxy>>& RenderProxies)
	{
		ComponentCount++;

		Type ComponentType = Component->GetType();
		m_Components[ComponentType].Emplace(Component);

		SharedPtr<TransformComponent> TransformComp = SharedPtr<TransformComponent>::DynamicCast(Component);
		if (TransformComp != nullptr)
		{
			if (TransformComp->GetRenderProxy() != nullptr)
			{
				RenderProxies.Add(TransformComp->GetRenderProxy());
			}

			for (auto& Child : TransformComp->GetChildren())
			{
				RegisterComponents(Child, RenderProxies);
			}
		}
	}

	void World::UnregisterComponents(EntityComponent* Component, Array<SharedPtr<RenderProxy>>& RenderProxies)
	{
		ComponentCount--;

		Type ComponentType = Component->GetType();
		Array<SharedPtr<EntityComponent>> Components = m_Components[ComponentType];
		Components.RemoveByPredicate([&](const SharedPtr<EntityComponent>& It) { return It.Get() == Component; });
		if (Components.Size() == 0)
		{
			m_Components.erase(ComponentType);
		}

		TransformComponent* TransformComp = dynamic_cast<TransformComponent*>(Component);
		if (TransformComp != nullptr)
		{
			if (TransformComp->GetRenderProxy() != nullptr)
			{
				RenderProxies.Add(TransformComp->GetRenderProxy());
			}

			for (auto& Child : TransformComp->GetChildren())
			{
				UnregisterComponents(Child.Get(), RenderProxies);
			}
		}
	}

	void World::UpdateRenderProxies(const Array<SharedPtr<RenderProxy>>& Add, const Array<SharedPtr<RenderProxy>>& Remove)
	{
		SharedPtr<SceneRenderProxy> SRP = m_SceneRenderProxy;
		if (SRP != nullptr && (Add.Size() > 0 || Remove.Size() > 0))
		{
			RenderThread::Enqueue([=](RHIContextImmediate& Context)
				{
					for (auto& RP : Add)
					{
						SRP->AddProxy(RP);
					}

					for (auto& RP : Remove)
					{
						SRP->RemoveProxy(RP);
					}
				});
		}
	}
}
