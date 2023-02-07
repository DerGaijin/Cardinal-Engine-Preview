#pragma once
#include "../Framework/Core/Headers.h"

namespace CE
{
	class World;
	class InputEvent;

	class CE_API GameInstance : public Singleton
	{
		friend class Engine;
	public:
		virtual bool Initialize() { return true; };
		
		virtual void BeginFrame() {};
		
		virtual void Tick(float DeltaTime);

		virtual void EndFrame() {};

		virtual void Terminate() {};

	public:
		virtual bool ProcessInputEvent(const InputEvent& Event);

	public:
		bool AddWorld(const SharedPtr<World>& World);

		bool RemoveWorld(const SharedPtr<World>& World);

		bool RemoveWorld(World* World);

		const Array<SharedPtr<World>>& Worlds() const
		{
			return m_Worlds;
		}

	private:
		void RenderWorlds(float DeltaTime);

	private:
		Array<SharedPtr<World>> m_Worlds;
	};
}
