#include "CardinalEngine-Editor/EngineEditor/EngineEditor.h"
#include "CardinalEngine/Framework/ModuleSystem/ModuleRegister.h"

namespace CE
{
	EngineEditor* EngineEditor::s_EditorInstance = nullptr;
	
	EngineEditor::EngineEditor()
	{
		CE_CHECK(s_EditorInstance == nullptr);
		s_EditorInstance = this;
	}

	void EngineEditor::Initialize()
	{
		ModuleRegister::Get().RegisterModule(Module);

		ImGuiInterface.Initialize();
	}

	void EngineEditor::BeginFrame(float DeltaTime)
	{
		ImGuiInterface.BeginFrame(DeltaTime);
	}

	void EngineEditor::Tick(float DeltaTime)
	{

	}

	void EngineEditor::EndFrame()
	{
		ImGuiInterface.EndFrame();
	}

	void EngineEditor::Terminate()
	{
		ImGuiInterface.Terminate();

		ModuleRegister::Get().UnregisterModule(Module);
	}

	EngineEditor::~EngineEditor()
	{
		CE_CHECK(s_EditorInstance == this);
		s_EditorInstance = nullptr;
	}

	EngineEditor& EngineEditor::Get()
	{
		CE_CHECK(s_EditorInstance != nullptr);
		return *s_EditorInstance;
	}
}
