#pragma once
#include "../Framework/Core/Headers.h"
#include "../Framework/Object/Object.h"
#include "../Framework/Reflection/Type.h"

namespace CE
{
	class GameInstance;
	class SceneRenderProxy;
	class RenderProxy;
	class Entity;
	class EntityComponent;

	class CE_API World : public Object
	{
		CE_OBJECT_BODY();
	private:
		friend class GameInstance;
		friend class Entity;
		friend class TransformComponent;
	
	public:
		static inline const Vec3f RightVector = Vec3f(1.0f, 0.0f, 0.0f);
		static inline const Vec3f ForwardVector = Vec3f(0.0f, 1.0f, 0.0f);
		static inline const Vec3f UpVector = Vec3f(0.0f, 0.0f, 1.0f);

	public:
		World();
	
		virtual void Tick(float DeltaTime);

		bool AddEntity(const SharedPtr<Entity>& Entity);

		bool RemoveEntity(const SharedPtr<Entity>& Entity);

		bool RemoveEntity(Entity* Entity);

	public:
		GameInstance* GameInstance() const
		{
			return m_GameInstance;
		}

		const Array<SharedPtr<Entity>>& Entities() const
		{
			return m_Entities;
		}

		const Map<Type, Array<SharedPtr<EntityComponent>>>& Components() const
		{
			return m_Components;
		}

		void IterateComponents(const Type& Type, const FunctionRef<bool(const SharedPtr<EntityComponent>&)>& Iter, bool UseInherited = true) const;

	public:
		std::atomic<uint64> ComponentCount = 0;

	private:
		void RegisterComponents(const SharedPtr<EntityComponent>& Component, Array<SharedPtr<RenderProxy>>& RenderProxies);

		void UnregisterComponents(EntityComponent* Component, Array<SharedPtr<RenderProxy>>& RenderProxies);

		void UpdateRenderProxies(const Array<SharedPtr<RenderProxy>>& Add, const Array<SharedPtr<RenderProxy>>& Remove);

	private:
		CE::GameInstance* m_GameInstance = nullptr;

		SharedPtr<SceneRenderProxy> m_SceneRenderProxy;

		Array<SharedPtr<Entity>> m_Entities;
		Map<Type, Array<SharedPtr<EntityComponent>>> m_Components;

#if CE_EDITOR
	public:
		bool IsEditorWorld = false;
		WeakPtr<Entity> SelectedEntity;
#endif
	};
}
