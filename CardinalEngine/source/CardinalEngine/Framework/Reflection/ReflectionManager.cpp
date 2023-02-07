#include "CardinalEngine/Framework/Reflection/ReflectionManager.h"
#include "CardinalEngine/Framework/Reflection/ReflectionModule.h"

namespace CE
{
	ReflectionManager::ReflectionManager()
	{
		m_Globals = SharedPtr<Detail_Reflection::TypeInfo>(new Detail_Reflection::TypeInfo{ ReflectionID(0), ETypeCategory::Void, "", 0 });
		m_GlobalsType = Type(m_Globals);
	}

	ReflectionManager& ReflectionManager::Get()
	{
		static ReflectionManager Inst;
		return Inst;
	}

	void ReflectionManager::EnableModule(ReflectionModule& Module)
	{
		// Check if the Module is already enabled
		CE_ASSERT(!m_EnabledModules.Contains(&Module), "Tried to enable a Module that is already Enabled");

		m_EnabledModules.Add(&Module);

		{
			std::lock_guard<std::mutex> Lock(m_TypeInfosMutex);
			
			// Add all Registered Types of the Module
			for (auto& TI : Module.m_TypeInfos)
			{
				auto EmplaceResult = m_TypesInfos.emplace(TI.first, TI.second);
				CE_ASSERT(EmplaceResult.second, "Type '%s' already Registered", TI.second->TypeName.Data());
			}
		}

		{
			std::lock_guard<std::mutex> Lock(m_GlobalsMutex);
			
			// Add All Static Global Methods of the Module
			for (auto& MethodIt : Module.m_GlobalsTypeInfo->Methods)
			{
				SharedPtr<Detail_Reflection::MethodInfo> MI = MethodIt.second.m_Info.Lock();
				if (MI != nullptr && MI->Wrapper != nullptr)
				{
					const Method& Found = Method::FindMethodOverload(m_Globals->Methods, MI->ID, MI->Wrapper->ArgumentTypes);
					CE_ASSERT(!Found.IsValid(), "Tried to register Global Method '%s' that is already registered", MI->Name.Data());

					m_Globals->Methods.emplace(MethodIt.first, MethodIt.second);
				}
			}

			// Add All Static Global Properties of the Module
			for (auto& PropertyIt : Module.m_GlobalsTypeInfo->Properties)
			{
				SharedPtr<Detail_Reflection::PropertyInfo> PI = PropertyIt.second.m_Info.Lock();
				if (PI != nullptr && PI->Wrapper != nullptr)
				{
					auto Found = m_Globals->Properties.find(PI->ID);
					CE_ASSERT(Found == m_Globals->Properties.end(), "Tried to register Global Property '%s' that is already registered", PI->Name.Data());

					m_Globals->Properties.emplace(PropertyIt.first, PropertyIt.second);
				}
			}
		}

		// Mark the Module as enabled
		Module.m_IsEnabled = true;
	}

	void ReflectionManager::DisableModule(ReflectionModule& Module)
	{
		// Check if the Module is enabled and is the Last Module that was enabled
		CE_ASSERT(m_EnabledModules.Size() != 0 && m_EnabledModules[m_EnabledModules.Size() - 1] == &Module, "Tried to disable a Module that is not the Last Enabled module");
		m_EnabledModules.RemoveAt(m_EnabledModules.Size() - 1);

		{
			std::lock_guard<std::mutex> Lock(m_TypeInfosMutex);

			// Remove all Types of the Module
			for (auto& TI : Module.m_TypeInfos)
			{
				m_TypesInfos.erase(TI.first);
			}
		}

		{
			std::lock_guard<std::mutex> Lock(m_GlobalsMutex);

			// Remove all Static Global Methods of the Module
			for (auto& MethodIt : Module.m_GlobalsTypeInfo->Methods)
			{
				SharedPtr<Detail_Reflection::MethodInfo> MI = MethodIt.second.m_Info.Lock();
				if (MI != nullptr && MI->Wrapper != nullptr)
				{
					auto Found = m_Globals->Methods.find(MI->ID);
					while (Found != m_Globals->Methods.end() && Found->first == MI->ID)
					{
						if (Found->second.DoArgumentTypesMatch(MI->Wrapper->ArgumentTypes))
						{
							m_Globals->Methods.erase(Found);
							break;
						}
						Found++;
					}
				}
			}

			// Remove all Static Global Properties of the Module
			for (auto& PropertyIt : Module.m_GlobalsTypeInfo->Properties)
			{
				SharedPtr<Detail_Reflection::PropertyInfo> PI = PropertyIt.second.m_Info.Lock();
				if (PI != nullptr && PI->Wrapper != nullptr)
				{
					auto Found = m_Globals->Properties.find(PI->ID);
					m_Globals->Properties.erase(Found);
				}
			}
		}

		// Mark the Module as not enabled
		Module.m_IsEnabled = false;
	}
}
