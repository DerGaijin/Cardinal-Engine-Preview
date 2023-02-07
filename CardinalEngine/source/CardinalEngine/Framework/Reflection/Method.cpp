#include "CardinalEngine/Framework/Reflection/Method.h"
#include "CardinalEngine/Framework/Reflection/Detail/MethodInfo.h"
#include "CardinalEngine/Framework/Reflection/Any.h"
#include "CardinalEngine/Framework/Reflection/Handle.h"
#include "CardinalEngine/Framework/Reflection/Argument.h"
#include "CardinalEngine/Framework/Reflection/ReflectionManager.h"

namespace CE
{
	const Method& Method::FindMethodOverload(MultiMap<ReflectionID, Method>& Container, const ReflectionID& ID, const Array<Type>& ArgumentTypes)
	{
		auto Found = Container.find(ID);
		while (Found != Container.end() && Found->first == ID)
		{
			if (Found->second.DoArgumentTypesMatch(ArgumentTypes))
			{
				return Found->second;
			}
			Found++;
		}

		static Method Empty;
		return Empty;
	}

	MultiMap<ReflectionID, Method> Method::All()
	{
		std::lock_guard<std::mutex> Lock(ReflectionManager::Get().m_GlobalsMutex);
		return ReflectionManager::Get().m_GlobalsType.Methods();
	}

	Method Method::Find(const ReflectionID& ID)
	{
		std::lock_guard<std::mutex> Lock(ReflectionManager::Get().m_GlobalsMutex);
		return ReflectionManager::Get().m_GlobalsType.GetMethod(ID);
	}

	Method Method::Find(const ReflectionID& ID, const Array<Type>& ArgumentTypes)
	{
		std::lock_guard<std::mutex> Lock(ReflectionManager::Get().m_GlobalsMutex);
		return ReflectionManager::Get().m_GlobalsType.GetMethod(ID, ArgumentTypes);
	}

	Any Method::StaticExecute(const ReflectionID& ID, const Array<Argument>& Arguments /*= {}*/)
	{
		bool Result = false;
		return StaticExecute(ID, Arguments, Result);
	}

	Any Method::StaticExecute(const ReflectionID& ID, const Array<Argument>& Arguments, bool& Result)
	{
		std::lock_guard<std::mutex> Lock(ReflectionManager::Get().m_GlobalsMutex);
		return ReflectionManager::Get().m_GlobalsType.ExecuteMethod(Handle(), ID, Arguments, Result);
	}

	Method::Method(const WeakPtr<Detail_Reflection::MethodInfo>& Info) : m_Info(Info)
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			m_ID = TI->ID;
		}
	}

	bool Method::IsValid() const
	{
		return !m_Info.Expired();
	}

	const ReflectionID& Method::ID() const
	{
		return m_ID;
	}

	const String& Method::Name() const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Name;
		}
		static String Empty;
		return Empty;
	}

	bool Method::IsStatic() const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr && TI->Wrapper != nullptr)
		{
			return TI->Wrapper->IsStatic();
		}
		return false;
	}

	size_t Method::ArgumentCount() const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr && TI->Wrapper != nullptr)
		{
			return TI->Wrapper->ArgumentTypes.Size();
		}
		return 0;
	}

	Array<Type> Method::ArgumentTypes() const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr && TI->Wrapper != nullptr)
		{
			return TI->Wrapper->ArgumentTypes;
		}
		return Array<Type>();
	}

	bool Method::DoArgumentTypesMatch(const Array<Type>& ArgumentTypes) const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr && TI->Wrapper != nullptr)
		{
			return TI->Wrapper->ArgumentTypes == ArgumentTypes;
		}
		return false;
	}

	Type Method::ReturnType() const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr && TI->Wrapper != nullptr)
		{
			return TI->Wrapper->ReturnType;
		}
		return Type();
	}

	Type Method::Owner() const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Owner;
		}
		return Type();
	}

	Any Method::Execute(const Array<Argument>& Arguments /*= {}*/) const
	{
		bool Result = false;
		return Execute(Handle(), Arguments, Result);
	}

	Any Method::Execute(const Array<Argument>& Arguments, bool& Result) const
	{
		return Execute(Handle(), Arguments, Result);
	}

	Any Method::Execute(const Handle& Handle, const Array<Argument>& Arguments /*= {}*/) const
	{
		bool Result = false;
		return Execute(Handle, Arguments, Result);
	}

	Any Method::Execute(const Handle& Handle, const Array<Argument>& Arguments, bool& Result) const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr && TI->Wrapper != nullptr)
		{
			return TI->Wrapper->Execute(Handle, Arguments, Result);
		}
		Result = false;
		return Any();
	}

	size_t Method::MetadataCount() const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Metas.size();
		}
		return 0;
	}

	Map<String, String> Method::Metadatas() const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Metas;
		}
		return Map<String, String>();
	}

	bool Method::HasMetadata(const String& Key) const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Metas.find(Key) != TI->Metas.end();
		}
		return false;
	}

	String Method::Metadata(const String& Key) const
	{
		SharedPtr<Detail_Reflection::MethodInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto Found = TI->Metas.find(Key);
			if (Found != TI->Metas.end())
			{
				return Found->second;
			}
		}
		return String();
	}
}
