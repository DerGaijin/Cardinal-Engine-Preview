#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine-Editor/EngineEditor/EngineEditor.h"

#include "../ProjectManagment/ProjectManagementGameInstance.h"

int wmain(int Argc, wchar_t* Argv[])
{
	using namespace CE;

	int ExitCode = 0;
	try
	{
		bool Restart = true;
		while (Restart)
		{
			// Initialize Framework
			Engine::InitializeFramework(Argc, Argv);
			
			// Create Engine Instance
			Engine* EngineInstance = new Engine();

			// Initialize Engine
			EngineInstance->Initialize();

			// Declare Project Instance
			Project* ProjectInstance = nullptr;

			// Create Editor Instance
			EngineEditor* EngineEditorInstance = new EngineEditor();

			// Initialize Editor
			EngineEditorInstance->Initialize();

			// Set Project Selection Game Instance
			EngineInstance->AddGameInstance(SharedPtr<GameInstance>(new CEE::ProjectManagementGameInstance(ProjectInstance)));

			// Main Loop
			while (!EngineInstance->IsExitRequested())
			{
				// Begin Engine Frame
				float DeltaTime = EngineInstance->BeginFrame();

				// Begin Editor Frame
				EngineEditorInstance->BeginFrame(DeltaTime);

				// Tick Engine
				EngineInstance->Tick(DeltaTime);
				
				// Tick Editor
				EngineEditorInstance->Tick(DeltaTime);

				// End Editor Frame
				EngineEditorInstance->EndFrame();

				// End Engine Frame
				EngineInstance->EndFrame();
			}

			// Terminate the Project
			EngineInstance->TerminateProject();

			// Destroy the Project
			delete(ProjectInstance);
			ProjectInstance = nullptr;

			// Terminate Editor
			EngineEditorInstance->Terminate();

			// Destroy Editor
			delete(EngineEditorInstance);
			EngineEditorInstance = nullptr;

			// Terminate Engine
			EngineInstance->Terminate();

			// Get ExitCode and Restart Flag
			Restart = EngineInstance->IsRestartRequested();
			ExitCode = EngineInstance->GetExitCode();

			// Delete Engine Instance
			delete(EngineInstance);
			EngineInstance = nullptr;

			// Terminate Framework
			Engine::TerminateFramework();
		}
	}
	catch (const std::exception& E)
	{
		throw;
	}
	return ExitCode;
}
