#pragma once
#include "../Framework/Core/Headers.h"
#include "../Application/Application.h"
#include "../Framework/Configs/Ini/IniDocument.h"
#include "EngineModule.h"
#include "InputManager.h"
#include "../Threading/TaskSystem.h"
#include "../AssetSystem/AssetSystem.h"

namespace CE
{
	class GameInstance;
	class Project;
	class DynamicRHI;

	enum class EEngineState
	{
		Running,
		Restart,
		Exit,
	};

	// This is the Heart of the engine
	class CE_API Engine : public Singleton
	{
	public:
		// Initializes the framework
		static void InitializeFramework(int Argc, wchar_t* Argv[]);

		// Default constructor
		Engine();

		// Initialize the engine
		void Initialize();

		// Initialize the project
		bool InitializeProject(Project* InProject, const std::filesystem::path& ProjectPath);

		// Begin frame
		float BeginFrame();

		// Tick engine
		void Tick(float DeltaTime);

		// End frame
		void EndFrame();

		// Terminate the project
		void TerminateProject();

		// Terminate the engine
		void Terminate();

		// Destructor
		~Engine();
		
		// Terminate the framework
		static void TerminateFramework();
	
	public:
		// Returns the Engine singleton
		static Engine& Get();
	
		// Request the engine to exit
		void RequestExit(int ExitCode = 0);

		// Request the engine to restart
		void RequestRestart();

		// Adds a game instance
		bool AddGameInstance(const SharedPtr<GameInstance>& GameInstance);

		// Removes a game instance by shared pointer
		bool RemoveGameInstance(const SharedPtr<GameInstance>& GameInstance);
		
		// Removes a game instance by pointer
		bool RemoveGameInstance(GameInstance* GameInstance);

	public:
		// Returns true if exit is requested
		bool IsExitRequested() const;

		// Returns true if restart is requested
		bool IsRestartRequested() const;

		// Returns the exit code
		int GetExitCode() const;

		// Returns the application
		Application* Application() const
		{
			return m_Application;
		}

		// Returns the current project
		Project* CurrentProject() const
		{
			return m_Project;
		}

		// Returns all game instances
		const Array<SharedPtr<GameInstance>>& GameInstances() const
		{
			return m_GameInstances;
		}

	public:
		IniDocument Config;

		EngineModule Module;

		InputManager InputManager;

		DynamicRHI& DynamicRHI() const { return *m_DynamicRHI; }

		TaskSystem TaskSystem;

		AssetSystem AssetSystem;

	private:
		static Engine* s_EngineInstance;
		static LogSink_Stream* s_ConsoleLogSink;
		static LogSink_Debugger* s_DebuggerLogSink;
		static LogSink_Stream* s_LogFileLogSink;
		static std::ofstream s_LogFileStream;

	private:
		std::atomic<EEngineState> m_State;
		std::atomic<int> m_ExitCode = 0;

		CE::Application* m_Application = nullptr;
		Array<SharedPtr<GameInstance>> m_GameInstances;
		Project* m_Project = nullptr;
		CE::DynamicRHI* m_DynamicRHI = nullptr;

		uint32 m_RenderSyncIndex = 0;
		Array<SharedPtr<TaskBase>> m_RenderSyncTasks;
	};
}
