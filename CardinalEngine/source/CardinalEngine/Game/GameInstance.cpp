#include "CardinalEngine/Game/GameInstance.h"
#include "CardinalEngine/Game/World.h"
#include "CardinalEngine/Game/RenderProxies/SceneRenderProxy.h"
#include "CardinalEngine/Game/Components/InputComponent.h"

#include "CardinalEngine/Engine/Engine.h"

namespace CE
{
	void GameInstance::Tick(float DeltaTime)
	{
		for (auto& World : m_Worlds)
		{
			World->Tick(DeltaTime);
		}
	}

	bool GameInstance::ProcessInputEvent(const InputEvent& Event)
	{
		for (auto& World : m_Worlds)
		{
			bool Handled = false;
			World->IterateComponents(InputComponent::StaticType(), [&](const SharedPtr<EntityComponent>& Comp)
				{
					InputComponent* InputComp = dynamic_cast<InputComponent*>(Comp.Get());
			if (InputComp != nullptr)
			{
				if (InputComp->ProcessInputEvent(Event))
				{
					Handled = true;
					return false;
				}
			}
			return true;
				});

			if (Handled)
			{
				return true;
			}
		}

		return false;
	}

	bool GameInstance::AddWorld(const SharedPtr<World>& World)
	{
		if (World == nullptr || World->m_GameInstance != nullptr)
		{
			return false;
		}

		m_Worlds.Emplace(World);
		World->m_GameInstance = this;

		return true;
	}

	bool GameInstance::RemoveWorld(const SharedPtr<World>& World)
	{
		return RemoveWorld(World.Get());
	}

	bool GameInstance::RemoveWorld(World* World)
	{
		if (World == nullptr || World->m_GameInstance != this)
		{
			return false;
		}

		m_Worlds.RemoveByPredicate([&](const SharedPtr<CE::World>& It) { return It.Get() == World; });
		World->m_GameInstance = nullptr;

		return true;
	}

	void GameInstance::RenderWorlds(float DeltaTime)
	{
		for (auto& World : m_Worlds)
		{
			SceneRenderData RenderData;
			RenderData.FrameBuffer;
			RenderData.EnvironmentBuffer;

			SharedPtr<SceneRenderProxy> SRP = World->m_SceneRenderProxy;
			RenderThread::Enqueue([=](RHIContextImmediate& Context)
				{
					SRP->RenderScene(Context, RenderData);
				});
		}
	}
}
