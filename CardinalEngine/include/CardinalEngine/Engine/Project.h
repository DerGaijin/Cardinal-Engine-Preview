#pragma once
#include "../Framework/Core/Headers.h"
#include "../Framework/Configs/Ini/IniDocument.h"

namespace CE
{
	class Module;
	class Engine;

	class CE_API Project : public Singleton
	{
		friend class Engine;
	public:
		// Returns the Project Module
		virtual Module& GetModule() const = 0;

	protected:
		// Initializes the Project
		virtual bool Initialize()
		{
			return true;
		}

		// Ticks the Project
		virtual void Tick(float DeltaTime)
		{

		}

		// Terminates the Project
		virtual void Terminate()
		{

		}

	public:
#if CE_EDITOR
		// Returns the Current Project Path
		const std::filesystem::path& GetProjectPath() const
		{
			return m_ProjectPath;
		}
#endif

	public:
		IniDocument Config;

	private:
#if CE_EDITOR
		std::filesystem::path m_ProjectPath;
#endif
	};
}

#if CE_EDITOR
#define CE_SET_PROJECT(PROJECT_CLASS) extern "C" CE_DYNAMIC_EXPORT CE::Project* CE_Create_Project() { return new PROJECT_CLASS(); }
#else
#define CE_SET_PROJECT(PROJECT_CLASS) CE::Project* CE_Create_Project() { return new PROJECT_CLASS(); }
#endif
