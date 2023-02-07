#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"
#include "../../CardinalEngine/Game/GameInstance.h"
#include "../UI/EditorUI.h"

namespace CE
{
	class Window;

	class CE_API EditorGameInstance : public GameInstance
	{
	public:
		EditorGameInstance();

		~EditorGameInstance();

	public:
		static EditorGameInstance* Get();

		virtual bool Initialize() override;

		virtual void BeginFrame() override;

		virtual void Tick(float DeltaTime) override;

		virtual void EndFrame() override;

		virtual void Terminate() override;

	public:
		SharedPtr<Window> EditorWindow() const
		{
			return m_EditorWindow;
		}

	public:
		EditorUI UI;

		WeakPtr<World> SelectedWorld;

	private:
		static EditorGameInstance* s_Instance;

		SharedPtr<Window> m_EditorWindow;
	};
}
