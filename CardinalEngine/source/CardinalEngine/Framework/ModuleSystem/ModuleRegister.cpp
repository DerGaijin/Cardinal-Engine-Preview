#include "CardinalEngine/Framework/ModuleSystem/ModuleRegister.h"
#include "CardinalEngine/Framework/ModuleSystem/Module.h"
#include "CardinalEngine/Framework/Platform/Platform.h"
#include "CardinalEngine/Framework/Reflection/ReflectionManager.h"

namespace CE
{
	ModuleRegister& ModuleRegister::Get()
	{
		static ModuleRegister Inst;
		return Inst;
	}

	bool ModuleRegister::RegisterModule(Module& Module)
	{
		if (m_ActiveModules.Contains(&Module))
		{
			return false;
		}

		Module.m_LibraryHandle = Platform::Get().GetLibraryFromAddress(&Module);

		for (auto& Mod : m_ActiveModules)
		{
			if (!Module.IsCompatibleWith(*Mod))
			{
				CE_LOG(ELog::Error, "Module '%s' is incompatible with '%s'", Module.Name().Data(), Mod->Name().Data());
				return false;
			}
		}

		m_ActiveModules.Add(&Module);

		ReflectionRegistration Registrator(Module.m_ReflectionModule);
		Module.RegisterReflection(Registrator);
		ReflectionManager::Get().EnableModule(Module.m_ReflectionModule);
		Module.Register();

		return true;
	}

	void ModuleRegister::UnregisterModule(Module& Module)
	{
		Module.Unregister();
		ReflectionManager::Get().DisableModule(Module.m_ReflectionModule);

		m_ActiveModules.Remove(&Module, 1);
	}
}
