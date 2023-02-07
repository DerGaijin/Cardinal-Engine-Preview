#pragma once
#include "../Framework/Core/Headers.h"
#include "../Engine/Engine.h"


extern CE::Project* CE_Create_Project();
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
			Project* ProjectInstance = CE_Create_Project();

			// Set Project Selection Game Instance
			EngineInstance->InitializeProject(ProjectInstance, "");

			// Main Loop
			while (!EngineInstance->IsExitRequested())
			{
				// Begin Engine Frame
				float DeltaTime = EngineInstance->BeginFrame();

				// Tick Engine
				EngineInstance->Tick(DeltaTime);

				// End Engine Frame
				EngineInstance->EndFrame();
			}

			// Terminate the Project
			EngineInstance->TerminateProject();

			// Destroy the Project
			delete(ProjectInstance);
			ProjectInstance = nullptr;

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
		//@TODO: Implement the Exception handler
		throw;
	}
	return ExitCode;
}
