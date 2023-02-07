#pragma once
#include "../Core/Headers.h"
#include "../Reflection/Registration.h"

namespace CE
{
	class ModuleRegister;

	class CE_API Module : public Singleton
	{
		friend class ModuleRegister;
	public:
	
		// Returns the Name
		virtual String Name() const = 0;
	
		// Returns the Version
		virtual VersionNumber Version() const = 0;
	
	protected:
		// For the User to register (eg. Asset Storages, ...)
		virtual void Register() {}

		// For the User to unregister
		virtual void Unregister() {}

		// Registers all Reflection Types
		virtual void RegisterReflection(ReflectionRegistration& Register) {}

		// Returns true if this Module is compatible with the Module
		virtual bool IsCompatibleWith(Module& Module)
		{
			return true;
		}

	public:
		// Returns the Dynamic Library Handle
		void* GetLibraryHandle() const
		{
			return m_LibraryHandle;
		}

	private:
		void* m_LibraryHandle = nullptr;
		ReflectionModule m_ReflectionModule;
	};
}
