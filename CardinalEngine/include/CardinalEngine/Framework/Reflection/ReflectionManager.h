#pragma once
#include "../Core/Headers.h"
#include "ReflectionID.h"
#include "Type.h"

namespace CE
{
	namespace Detail_Reflection
	{
		struct TypeInfo;
	}
	class ReflectionModule;

	// Contains all Registered Types
	class CE_API ReflectionManager : public Singleton
	{
		friend class Type;
		friend class Method;
		friend class Property;
		friend class ReflectionModule;

		// Private Constructor
		ReflectionManager();

	public:
		// Returns the Singleton Instance
		static ReflectionManager& Get();

		// Enables a Module
		void EnableModule(ReflectionModule& Module);

		// Disables a Module
		void DisableModule(ReflectionModule& Module);

	private:
		Array<ReflectionModule*> m_EnabledModules;

		mutable std::mutex m_TypeInfosMutex;
		Map<ReflectionID, Type> m_TypesInfos;

		mutable std::mutex m_GlobalsMutex;
		SharedPtr<Detail_Reflection::TypeInfo> m_Globals;
		Type m_GlobalsType;
	};
}
