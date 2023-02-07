#include "CardinalEngine-Editor/EngineEditor/EditorGameInstance.h"
#include "CardinalEngine-Editor/EngineEditor/EngineEditor.h"
#include "CardinalEngine/Application/Application.h"
#include "CardinalEngine/Application/Window.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Engine/Project.h"
#include "CardinalEngine/Framework/ModuleSystem/Module.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/RHI/Swapchain.h"


namespace CE
{
	EditorGameInstance* EditorGameInstance::s_Instance = nullptr;

	EditorGameInstance::EditorGameInstance()
	{
		CE_CHECK(s_Instance == nullptr);
		s_Instance = this;
	}

	EditorGameInstance::~EditorGameInstance()
	{
		CE_CHECK(s_Instance == this);
		s_Instance = nullptr;
	}

	EditorGameInstance* EditorGameInstance::Get()
	{
		return s_Instance;
	}

	bool EditorGameInstance::Initialize()
	{
		Text ProjectName;
		Project* CurrentProject = Engine::Get().CurrentProject();
		if (CurrentProject != nullptr)
		{
			ProjectName = Text::Convert(Engine::Get().CurrentProject()->GetModule().Name());
		}

		m_EditorWindow = SharedPtr<Window>(Engine::Get().Application()->CreateWindow(Text::Format(L"Cardinal Engine - %s", ProjectName.Data()), Vec2ui(1200, 800), EWindowDisplayMode::Maximized));
		Engine::Get().DynamicRHI().CreateSwapchain(m_EditorWindow.Get());

		EngineEditor::Get().ImGuiInterface.SetWindow(m_EditorWindow);


		InputManager& IM = Engine::Get().InputManager;

		InputEvent_Axis& CEE_MoveForward = IM.AddAxisEvent("CEE_MoveForward");
		CEE_MoveForward.Set(EInput::Keyboard_W, 1.0f);
		CEE_MoveForward.Set(EInput::Keyboard_S, -1.0f);

		InputEvent_Axis& CEE_MoveRight = IM.AddAxisEvent("CEE_MoveRight");
		CEE_MoveRight.Set(EInput::Keyboard_D, 1.0f);
		CEE_MoveRight.Set(EInput::Keyboard_A, -1.0f);

		InputEvent_Axis& CEE_MoveUp = IM.AddAxisEvent("CEE_MoveUp");
		CEE_MoveUp.Set(EInput::Keyboard_Space, 1.0f);
		CEE_MoveUp.Set(EInput::Keyboard_C, -1.0f);
		
		InputEvent_Axis& CEE_LookRight = IM.AddAxisEvent("CEE_LookRight");
		CEE_LookRight.Set(EInput::Mouse_X, -1.0f);

		InputEvent_Axis& CEE_LookUp = IM.AddAxisEvent("CEE_LookUp");
		CEE_LookUp.Set(EInput::Mouse_Y, 1.0f);

		return true;
	}

	void EditorGameInstance::BeginFrame()
	{
		SharedPtr<Window> EditorWindow = m_EditorWindow;
		RenderThread::Enqueue([EditorWindow](RHIContextImmediate& Context)
			{
				Context.PrepareSwapchain(EditorWindow->GetSwapchain());

				SharedPtr<RHIImage> RT = EditorWindow->GetSwapchain()->CurrentImage();

				Context.BeginRenderpass({ RT }, nullptr);
				Context.ClearRenderTarget(RT, Color::FromRGB(0.1f, 0.1f, 0.1f));
				Context.EndRenderpass();
			});
	}

	void EditorGameInstance::Tick(float DeltaTime)
	{
		GameInstance::Tick(DeltaTime);

		if (m_EditorWindow == nullptr || m_EditorWindow->IsCloseRequested())
		{
			Engine::Get().RequestExit();
		}

		if (SelectedWorld.Expired() && Worlds().Size() > 0)
		{
			SelectedWorld = Worlds()[0];
		}

		UI.Tick(DeltaTime);
		UI.Draw(DeltaTime);
	}

	void EditorGameInstance::EndFrame()
	{
		SharedPtr<Window> EditorWindow = m_EditorWindow;
		RenderThread::Enqueue([EditorWindow](RHIContextImmediate& Context)
			{
				Context.PresentSwapchain(EditorWindow->GetSwapchain());
			});
	}

	void EditorGameInstance::Terminate()
	{
		m_EditorWindow.Reset();
	}
}
