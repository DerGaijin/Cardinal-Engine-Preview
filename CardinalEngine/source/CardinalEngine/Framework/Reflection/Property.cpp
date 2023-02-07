#include "CardinalEngine/Framework/Reflection/Property.h"
#include "CardinalEngine/Framework/Reflection/Detail/PropertyInfo.h"
#include "CardinalEngine/Framework/Reflection/Handle.h"
#include "CardinalEngine/Framework/Reflection/Argument.h"
#include "CardinalEngine/Framework/Reflection/ReflectionManager.h"

namespace CE
{
	Map<ReflectionID, Property> Property::All()
	{
		std::lock_guard<std::mutex> Lock(ReflectionManager::Get().m_GlobalsMutex);
		return ReflectionManager::Get().m_GlobalsType.Properties();
	}

	Property Property::Find(const ReflectionID& ID)
	{
		std::lock_guard<std::mutex> Lock(ReflectionManager::Get().m_GlobalsMutex);
		return ReflectionManager::Get().m_GlobalsType.GetProperty(ID);
	}

	Handle Property::StaticGetValue(const ReflectionID& ID)
	{
		std::lock_guard<std::mutex> Lock(ReflectionManager::Get().m_GlobalsMutex);
		return ReflectionManager::Get().m_GlobalsType.GetPropertyValue(Handle(), ID);
	}

	bool Property::StaticSetValue(const ReflectionID& ID, const Argument& Value)
	{
		std::lock_guard<std::mutex> Lock(ReflectionManager::Get().m_GlobalsMutex);
		return ReflectionManager::Get().m_GlobalsType.SetPropertyValue(Handle(), ID, Value);
	}

	Property::Property(const WeakPtr<Detail_Reflection::PropertyInfo>& Info) : m_Info(Info)
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr)
		{
			m_ID = PI->ID;
		}
	}

	bool Property::IsValid() const
	{
		return !m_Info.Expired();
	}

	const ReflectionID& Property::ID() const
	{
		return m_ID;
	}

	const String& Property::Name() const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr)
		{
			return PI->Name;
		}
		static String Empty;
		return Empty;
	}

	bool Property::IsStatic() const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr && PI->Wrapper != nullptr)
		{
			return PI->Wrapper->IsStatic();
		}
		return false;
	}

	bool Property::IsReadOnly() const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr && PI->Wrapper != nullptr)
		{
			return PI->Wrapper->IsReadOnly();
		}
		return false;
	}

	Type Property::Type() const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr && PI->Wrapper != nullptr)
		{
			return PI->Wrapper->PropType;
		}
		return CE::Type();
	}

	Type Property::Owner() const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr)
		{
			return PI->Owner;
		}
		return CE::Type();
	}

	Handle Property::GetValue() const
	{
		return GetValue(Handle());
	}

	Handle Property::GetValue(const Handle& Handle) const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr && PI->Wrapper != nullptr)
		{
			return PI->Wrapper->Get(Handle);
		}
		return CE::Handle();
	}

	bool Property::SetValue(const Argument& Value) const
	{
		return SetValue(Handle(), Value);
	}

	bool Property::SetValue(const Handle& Handle, const Argument& Value) const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr && PI->Wrapper != nullptr)
		{
			return PI->Wrapper->Set(Handle, Value);
		}
		return false;
	}

	size_t Property::MetadataCount() const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr)
		{
			return PI->Metas.size();
		}
		return 0;
	}

	Map<String, String> Property::Metadatas() const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr)
		{
			return PI->Metas;
		}
		return Map<String, String>();
	}

	bool Property::HasMetadata(const String& Key) const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr)
		{
			return PI->Metas.find(Key) != PI->Metas.end();
		}
		return false;
	}

	String Property::Metadata(const String& Key) const
	{
		SharedPtr<Detail_Reflection::PropertyInfo> PI = m_Info.Lock();
		if (PI != nullptr)
		{
			auto Found = PI->Metas.find(Key);
			if (Found != PI->Metas.end())
			{
				return Found->second;
			}
		}
		return String();
	}
}
