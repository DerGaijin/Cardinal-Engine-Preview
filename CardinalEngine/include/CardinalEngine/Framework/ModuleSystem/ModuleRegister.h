#pragma once
#include "../Core/Headers.h"

namespace CE
{
	class Module;

	class CE_API ModuleRegister : public Singleton
	{
		// Private Singleton Constructor
		ModuleRegister() = default;
	public:
		// Returns the Singleton Instance
		static ModuleRegister& Get();

		// Registers and Initializes a Module
		bool RegisterModule(Module& Module);

		// Terminates and Unregisters a Module
		void UnregisterModule(Module& Module);

		// Returns all Registered Modules
		const Array<Module*>& ActiveModules() const
		{
			return m_ActiveModules;
		}

	private:
		Array<Module*> m_ActiveModules;
	};
}
