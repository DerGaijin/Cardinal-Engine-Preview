#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Game/GameInstance.h"
#include "CardinalEngine/Engine/Project.h"

#include "CardinalEngine/Framework/Platform/Platform.h"
#include "CardinalEngine/Application/Application.h"
#include "CardinalEngine/FileSystem/Paths.h"
#include "CardinalEngine/Framework/ModuleSystem/ModuleRegister.h"
#include "CardinalEngine/Framework/ModuleSystem/Module.h"
#include "CardinalEngine/Framework/Reflection/Type.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Framework/Configs/Ini/IniDocumentParser.h"

#include <iostream>

namespace Utils
{
	using namespace CE;

	bool LoadIniConfig(const std::filesystem::path& Path, IniDocument& Config, const String& Description)
	{
		std::ifstream IniDocumentFile = std::ifstream(Path, std::ios::binary);
		if (IniDocumentFile.is_open())
		{
			IniDocumentParser IniParser(Config);
			IniParser.SetSource(&IniDocumentFile);
			ConfigParseException Error;
			if (!IniParser.Parse(&Error))
			{
				CE_LOG(ELog::Warning, "Failed to parse %s Config: %s", Description.Data(), Error.what());
				IniDocumentFile.close();
				return false;
			}
			IniDocumentFile.close();
		}
		else
		{
			CE_LOG(ELog::Warning, "Could not find %s Config at '%s'", Description.Data(), Path.u8string().c_str());
			return false;
		}
		return true;
	}
}

namespace CE
{
	Engine* Engine::s_EngineInstance = nullptr;
	LogSink_Stream* Engine::s_ConsoleLogSink = nullptr;
	LogSink_Debugger* Engine::s_DebuggerLogSink = nullptr;
	LogSink_Stream* Engine::s_LogFileLogSink = nullptr;
	std::ofstream Engine::s_LogFileStream;

	void Engine::InitializeFramework(int Argc, wchar_t* Argv[])
	{
		// Parse Start Arguments
		StartArgument::Parse(Argc, Argv);

		// Create Platform Application
		Application::Create();

		// Set Engine Resources Path
#if CE_EDITOR
		Paths::m_EngineResourcesPath = std::filesystem::absolute(Paths::ApplicationDirectory() / "../../../CardinalEngine - Resources").lexically_normal();
#else
		Paths::m_EngineResourcesPath = std::filesystem::absolute(Paths::ApplicationDirectory() / "CardinalEngine - Resources").lexically_normal();
#endif

		// Get the Start Argument Override for the Engine Resources
		StartArgument::OverridePath(Paths::m_EngineResourcesPath, L"EngineResources");

		// Set the Editor Resource and Work Directory Paths
#if CE_EDITOR
		if (Paths::m_EditorResourcesPath.empty())
		{
			Paths::m_EditorResourcesPath = std::filesystem::absolute(Paths::ApplicationDirectory() / "../../../CardinalEngine-Editor - Resources").lexically_normal();
		}

		if (Paths::m_EditorWorkDirectoryPath.empty())
		{
			Paths::m_EditorWorkDirectoryPath = std::filesystem::absolute(Paths::ApplicationDirectory() / "../../../CardinalEngine-Editor - Work Directory").lexically_normal();

			// Get the Start Argument Override for the Editor Work Directory
			StartArgument::OverridePath(Paths::m_EditorWorkDirectoryPath, L"EditorWorkDirectory");
		}
#endif

#if CE_STANDALONE
		Paths::m_ProjectWorkDirectoryPath = std::filesystem::absolute(Paths::ApplicationDirectory() / "Work Directory").lexically_normal();
#endif

		// Get the Start Argument Override for the Project Work Directory
		StartArgument::OverridePath(Paths::m_ProjectWorkDirectoryPath, L"WorkDirectory");

		// Create Log Sinks
		s_ConsoleLogSink = new LogSink_Stream(std::cout);
		s_DebuggerLogSink = new LogSink_Debugger();
		std::filesystem::path LogFilePath = (Paths::WorkDirectory() / "Logs/Cardinal Engine.log").lexically_normal();
		std::filesystem::create_directories(LogFilePath.parent_path());
		s_LogFileStream.open(LogFilePath);
		if (s_LogFileStream.is_open())
		{
			s_LogFileLogSink = new LogSink_Stream(s_LogFileStream);
			s_LogFileLogSink->Format = "[{HOUR}:{MINUTE}:{SECOND}][{THREADID}] {CATEGORY}: {MSG}";
		}

		// Add Log Sinks
		Log::AddSink(s_ConsoleLogSink);
		Log::AddSink(s_DebuggerLogSink);
		Log::AddSink(s_LogFileLogSink);

		
		// Log Engine Version
		CE_LOG(ELog::Info, "Cardinal Engine Started (Version: %i.%i.%i)", CE_VERSION_MAJOR, CE_VERSION_MINOR, CE_VERSION_PATCH);
	}

	Engine::Engine()
	{
		CE_CHECK(s_EngineInstance == nullptr);
		s_EngineInstance = this;
	}

	void Engine::Initialize()
	{
		std::chrono::high_resolution_clock::time_point InitStart = std::chrono::high_resolution_clock::now();

		// Profile the Main Thread
		CE_PROFILE_THREAD("Main Thread");

		// Initialize Platform Application
		m_Application = Application::Create();
		CE_CHECK(m_Application->Initialize());
		
		// Load Engine Config
#if CE_EDITOR
		std::filesystem::path EngineConfigPath = (Paths::EditorWorkDirectory() / "Configs/Engine.ini").lexically_normal();
#else
		std::filesystem::path EngineConfigPath = (Paths::ProjectWorkDirectory() / "Configs/Engine.ini").lexically_normal();
#endif
		Utils::LoadIniConfig(EngineConfigPath, Config, "Engine");

		// Initialize DynamicRHI
		ERHIType ChosenRHIType = DynamicRHI::ChooseRHIType();
		m_DynamicRHI = DynamicRHI::Create(ChosenRHIType);
		CE_CHECK(m_DynamicRHI != nullptr);
		CE_CHECK(m_DynamicRHI->Load());
		CE_CHECK(m_DynamicRHI->Initialize(RHIConfig()));
		m_RenderSyncTasks.AddDefaulted(m_DynamicRHI->Config().FramesInFlight);

		// Print RHI Type
		String ChosenRHITypeStr = "None";
		switch (ChosenRHIType)
		{
		case CE::ERHIType::Vulkan:
			ChosenRHITypeStr = "Vulkan";
			break;
		case CE::ERHIType::DirectX12:
			ChosenRHITypeStr = "DirectX12";
			break;
		case CE::ERHIType::DirectX11:
			ChosenRHITypeStr = "DirectX11";
			break;
		}
		CE_LOG(ELog::Info, "Loaded and Initialized '%s' RHI on Device '%S'", ChosenRHITypeStr.Data(), m_DynamicRHI->Capabilities().DeviceName.Data());


		// Initialize TaskSystem
		CE_CHECK(TaskSystem.Initialize());

		// Initialize AssetSystem
		CE_CHECK(AssetSystem.Initialize());

		// Register Engine Module
		CE_CHECK(ModuleRegister::Get().RegisterModule(Module));

		// Log Engine Start Completed
		float StartUpMs = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::high_resolution_clock::now() - InitStart).count();
		CE_LOG(ELog::Info, "Engine Initialization Completed (Took %.2f ms)", StartUpMs);
	}

	bool Engine::InitializeProject(Project* InProject, const std::filesystem::path& ProjectPath)
	{
		CE_CHECK(m_Project == nullptr);
		m_Project = InProject;

#if CE_EDITOR
		// Set Project Path
		m_Project->m_ProjectPath = ProjectPath;

		// Set Project Path
		Paths::m_ProjectDirectoryPath = InProject->GetProjectPath().parent_path();

		// Set Project Work Directory
		if (Paths::m_ProjectWorkDirectoryPath.empty())
		{
			Paths::m_ProjectWorkDirectoryPath = std::filesystem::absolute(Paths::ProjectDirectory() / "Work Directory").lexically_normal();
		}
#endif

		// Load Project Config
		std::filesystem::path ProjectConfigPath = (Paths::ProjectWorkDirectory() / "Configs/Project.ini").lexically_normal();
		Utils::LoadIniConfig(ProjectConfigPath, m_Project->Config, "Project");

		// Register Project Module
		if (!ModuleRegister::Get().RegisterModule(m_Project->GetModule()))
		{
			return false;
		}

		// Initialize Project
		if (!m_Project->Initialize())
		{
			return false;
		}

		CE_LOG(ELog::Info, "Initialized Project '%s'", m_Project->GetModule().Name().Data());

		return true;
	}

	float Engine::BeginFrame()
	{
		// Process Profiler
		Profiler::Get().ProcessFrame();

		// Profile Begin Frame
		CE_PROFILE_BEGIN_HIERARCHY("Frame");

		// Calculate Delta Time
		static auto LastTime = std::chrono::high_resolution_clock::now();
		auto Now = std::chrono::high_resolution_clock::now();
		float DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(Now - LastTime).count() / 1000000.0f;
		LastTime = Now;

		// Clear Axis Input Values
		InputManager.ClearAxisValues();

		// Tick Application
		m_Application->Tick(DeltaTime);

		// Update Axis Input Values
		InputManager.UpdateAxisValues();

		// Tick Dynamic RHI
		m_DynamicRHI->Tick(DeltaTime);

		// Tick Asset System
		AssetSystem.Tick(DeltaTime);

		// Begin Game Instances
		Array<SharedPtr<GameInstance>> GameInstances = m_GameInstances;
		for (auto& GI : GameInstances)
		{
			GI->BeginFrame();
		}

		return DeltaTime;
	}

	void Engine::Tick(float DeltaTime)
	{
		// Tick Project
		if (m_Project != nullptr)
		{
			m_Project->Tick(DeltaTime);
		}

		// Tick Game Instances
		Array<SharedPtr<GameInstance>> GameInstances = m_GameInstances;
		for (auto& GI : GameInstances)
		{
			GI->RenderWorlds(DeltaTime);
			GI->Tick(DeltaTime);
		}
	}

	void Engine::EndFrame()
	{
		// End Game Instances
		Array<SharedPtr<GameInstance>> GameInstances = m_GameInstances;
		for (auto& GI : GameInstances)
		{
			GI->EndFrame();
		}

		// Process Pending Main Thread Task from the TaskSystem
		TaskSystem.ProcessQueueUntilIdle();

		// Wait for Render Thread
		m_RenderSyncTasks[m_RenderSyncIndex] = RenderThread::Enqueue([](RHIContextImmediate&) {});
		m_RenderSyncIndex = m_RenderSyncIndex + 1 >= m_RenderSyncTasks.Size() ? 0 : m_RenderSyncIndex + 1;
		if (m_RenderSyncTasks[m_RenderSyncIndex] != nullptr)
		{
			m_RenderSyncTasks[m_RenderSyncIndex]->Wait();
		}

		// Flush Logging
		Log::Flush();

		// End Tick Profile Block
		CE_PROFILE_END_HIERARCHY();
	}

	void Engine::TerminateProject()
	{
		if (m_Project != nullptr)
		{
			// Terminate the Project
			m_Project->Terminate();

			// Unregister Project Module
			ModuleRegister::Get().UnregisterModule(m_Project->GetModule());

			// Save Project Config
			String ConfigString = m_Project->Config.Write();
			if (ConfigString.Size() > 0)
			{
				std::filesystem::path ProjectConfigPath = (Paths::ProjectWorkDirectory() / "Configs/Project.ini").lexically_normal();

				std::ofstream IniDocumentFile = std::ofstream(ProjectConfigPath, std::ios::binary);
				if (IniDocumentFile.is_open())
				{
					IniDocumentFile.write(ConfigString.Data(), ConfigString.Size());
					IniDocumentFile.close();
				}
				else
				{
					CE_LOG(ELog::Warning, "Failed to save Project Config to '%s'", ProjectConfigPath.u8string().c_str());
				}
			}

			m_Project = nullptr;
		}
	}

	void Engine::Terminate()
	{
		// Clear Remaining Game Instances
		Array<SharedPtr<GameInstance>> GameInstances = m_GameInstances;
		for (auto& GI : GameInstances)
		{
			GI->Terminate();
		}
		m_GameInstances.Clear();

		// Unregister Engine Module
		ModuleRegister::Get().UnregisterModule(Module);

		// Terminate AssetSystem
		AssetSystem.Terminate();

		// Terminate TaskSystem
		TaskSystem.Terminate();

		// Terminate DynamicRHI
		m_DynamicRHI->Terminate();
		m_DynamicRHI->Unload();
		delete(m_DynamicRHI);
		m_DynamicRHI = nullptr;

		// Save Engine Config
		String ConfigString = Config.Write();
		if (ConfigString.Size() > 0)
		{
#if CE_EDITOR
			std::filesystem::path EngineConfigPath = Paths::EditorWorkDirectory();
#else
			std::filesystem::path EngineConfigPath = Paths::ProjectWorkDirectory();
#endif
			EngineConfigPath = (EngineConfigPath / "Configs/Engine.ini").lexically_normal();

			std::ofstream IniDocumentFile = std::ofstream(EngineConfigPath, std::ios::binary);
			if (IniDocumentFile.is_open())
			{
				IniDocumentFile.write(ConfigString.Data(), ConfigString.Size());
				IniDocumentFile.close();
			}
			else
			{
				CE_LOG(ELog::Warning, "Failed to save Engine Config to '%s'", EngineConfigPath.u8string().c_str());
			}
		}

		// Terminate Platform Application
		m_Application->Terminate();
		delete(m_Application);
		m_Application = nullptr;

	}

	Engine::~Engine()
	{
		CE_CHECK(s_EngineInstance == this);
		s_EngineInstance = nullptr;
	}

	void Engine::TerminateFramework()
	{
		// Check all Types unregistered
		CE_CHECK(Type::All().size() == 0);

		// Check all Modules unregistered
		CE_CHECK(ModuleRegister::Get().ActiveModules().Size() == 0);

		// Reset Profiler Data
		Profiler::Get().Reset();

		// Flush Logging
		Log::Flush();

		// Remove Log Sinks
		Log::RemoveSink(s_LogFileLogSink);
		s_LogFileStream.close();
		Log::RemoveSink(s_DebuggerLogSink);
		Log::RemoveSink(s_ConsoleLogSink);

		// Destroy Log Sinks
		delete(s_LogFileLogSink);
		s_LogFileLogSink = nullptr;
		delete(s_DebuggerLogSink);
		s_DebuggerLogSink = nullptr;
		delete(s_ConsoleLogSink);
		s_ConsoleLogSink = nullptr;

		// Clear Paths
		Paths::m_EngineResourcesPath.clear();
		Paths::m_ProjectWorkDirectoryPath.clear();
#if CE_EDITOR
		Paths::m_ProjectDirectoryPath.clear();
		Paths::m_EditorResourcesPath.clear();
		Paths::m_EditorWorkDirectoryPath.clear();
#endif
	}

	Engine& Engine::Get()
	{
		CE_ASSERT(s_EngineInstance != nullptr, "Called Engine::Get() before a Engine was created");
		return *s_EngineInstance;
	}

	void Engine::RequestExit(int ExitCode /*= 0*/)
	{
		m_State.store(EEngineState::Exit);
		int Expected = 0;
		m_ExitCode.compare_exchange_strong(Expected, ExitCode);
	}

	void Engine::RequestRestart()
	{
		EEngineState Expected = EEngineState::Running;
		m_State.compare_exchange_strong(Expected, EEngineState::Restart);
	}

	bool Engine::AddGameInstance(const SharedPtr<GameInstance>& GI)
	{
		if (!m_GameInstances.Contains(GI))
		{
			m_GameInstances.Emplace(GI);
			if (GI->Initialize())
			{
				return true;
			}
			RemoveGameInstance(GI);
		}
		return false;
	}

	bool Engine::RemoveGameInstance(const SharedPtr<GameInstance>& GI)
	{
		if (m_GameInstances.Remove(GI) > 0)
		{
			GI->Terminate();
			return true;
		}
		return false;
	}

	bool Engine::RemoveGameInstance(GameInstance* GI)
	{
		if (m_GameInstances.RemoveByPredicate([&](const SharedPtr<GameInstance>& It) { return It.Get() == GI; }) > 0)
		{
			GI->Terminate();
			return true;
		}
		return false;
	}

	bool Engine::IsExitRequested() const
	{
		return m_State.load() != EEngineState::Running;
	}

	bool Engine::IsRestartRequested() const
	{
		return m_State.load() == EEngineState::Restart;
	}

	int Engine::GetExitCode() const
	{
		return m_ExitCode.load();
	}
}
