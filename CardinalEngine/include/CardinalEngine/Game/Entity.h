#pragma once
#include "../Framework/Core/Headers.h"
#include "../Framework/Object/Object.h"

namespace CE
{
	class World;
	class EntityComponent;
	class TransformComponent;

	class CE_API Entity : public Object
	{
		CE_OBJECT_BODY();
	private:
		friend class World;
	
	public:
		Entity();

		Entity(const SharedPtr<TransformComponent>& Root);

		virtual void Tick(float DeltaTime) {}

		bool SetRootComponent(const SharedPtr<TransformComponent>& NewRoot);

		bool AddComponent(const SharedPtr<EntityComponent>& Component);

		bool RemoveComponent(EntityComponent* Component);

		bool RemoveComponent(const SharedPtr<EntityComponent>& Component);

	public:
		SharedPtr<World> World() const
		{
			return m_World.Lock();
		}

		const SharedPtr<TransformComponent>& GetRootComponent() const
		{
			return m_RootComponent;
		}

		const Array<SharedPtr<EntityComponent>>& GetComponents() const
		{
			return m_Components;
		}

#if CE_EDITOR
	protected:
		friend class MainEditor;
		virtual void DrawInspector() {}
#endif

	private:
		WeakPtr<CE::World> m_World;

		SharedPtr<TransformComponent> m_RootComponent;
		Array<SharedPtr<EntityComponent>> m_Components;

#if CE_EDITOR
	public:
		bool IsEditorEntity = false;
		WeakPtr<EntityComponent> SelectedComponent;
#endif
	};
}
