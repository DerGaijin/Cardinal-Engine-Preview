#include "CardinalEngine/Framework/Reflection/Type.h"
#include "CardinalEngine/Framework/Reflection/ReflectionManager.h"
#include "CardinalEngine/Framework/Reflection/Detail/TypeInfo.h"
#include "CardinalEngine/Framework/Reflection/Detail/MethodInfo.h"
#include "CardinalEngine/Framework/Reflection/Method.h"
#include "CardinalEngine/Framework/Reflection/Any.h"
#include "CardinalEngine/Framework/Reflection/Handle.h"
#include "CardinalEngine/Framework/Reflection/Argument.h"

namespace CE
{
	Type Type::Find(const ReflectionID& ID)
	{
		std::lock_guard<std::mutex> Lock(ReflectionManager::Get().m_TypeInfosMutex);
		auto Found = ReflectionManager::Get().m_TypesInfos.find(ID);
		if (Found != ReflectionManager::Get().m_TypesInfos.end())
		{
			return Found->second;
		}
		return Type();
	}

	Map<ReflectionID, Type> Type::All()
	{
		std::lock_guard<std::mutex> Lock(ReflectionManager::Get().m_TypeInfosMutex);
		return ReflectionManager::Get().m_TypesInfos;
	}

	Type::Type(const WeakPtr<Detail_Reflection::TypeInfo>& Info) : m_Info(Info)
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			m_ID = TI->ID;
		}
	}

	bool Type::IsValid() const
	{
		return !m_Info.Expired();
	}

	const ReflectionID& Type::ID() const
	{
		return m_ID;
	}

	const String& Type::TypeName() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->TypeName;
		}
		static String Empty;
		return Empty;
	}

	ETypeCategory Type::Category() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Category;
		}
		return ETypeCategory::Void;
	}

	size_t Type::Size() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Size;
		}
		return 0;
	}

	Type Type::Underlying() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Underlying;
		}
		return Type();
	}

	size_t Type::ConstructorCount() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Constructors.Size();
		}
		return 0;
	}

	Array<Method> Type::Constructors() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Constructors;
		}
		return Array<Method>();
	}

	Method Type::GetConstructor(const Array<Type>& ArgumentTypes /*= {}*/) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			for (auto& Con : TI->Constructors)
			{
				if (Con.DoArgumentTypesMatch(ArgumentTypes))
				{
					return Con;
				}
			}
		}
		return Method();
	}

	Any Type::Construct(const Array<Argument>& Arguments /*= {}*/) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			for (auto& Con : TI->Constructors)
			{
				Any Result = Con.Execute(Arguments);
				if (Result.IsValid())
				{
					return Result;
				}
			}
		}
		return Any();
	}

	size_t Type::BaseCount() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Bases.Size();
		}
		return 0;
	}

	Array<Type> Type::Bases() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Bases;
		}
		return Array<Type>{};
	}

	bool Type::IsBaseOf(const Type& Child) const
	{
		return Child.IsChildOf(*this);
	}

	bool Type::IsChildOf(const Type& Base) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Bases.Contains(Base);
		}
		return false;
	}

	size_t Type::MethodCount() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Methods.size();
		}
		return 0;
	}

	MultiMap<ReflectionID, Method> Type::Methods() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Methods;
		}
		return MultiMap<ReflectionID, Method>();
	}

	Method Type::GetMethod(const ReflectionID& ID) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto Found = TI->Methods.find(ID);
			if (Found != TI->Methods.end())
			{
				return Found->second;
			}
		}
		return Method();
	}

	Method Type::GetMethod(const ReflectionID& ID, const Array<Type>& ArgumentTypes) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return Method::FindMethodOverload(TI->Methods, ID, ArgumentTypes);
		}
		return Method();
	}

	Any Type::ExecuteMethod(const Handle& Handle, const ReflectionID& ID, const Array<Argument>& Arguments /*= {}*/) const
	{
		bool Result = false;
		return ExecuteMethod(Handle, ID, Arguments, Result);
	}

	Any Type::ExecuteMethod(const Handle& Handle, const ReflectionID& ID, const Array<Argument>& Arguments, bool& Result) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto Found = TI->Methods.find(ID);
			while (Found != TI->Methods.end() && Found->second.ID() == ID)
			{
				Any AnyResult = Found->second.Execute(Handle, Arguments, Result);
				if (Result)
				{
					return AnyResult;
				}

				Found++;
			}
		}
		Result = false;
		return Any();
	}

	size_t Type::PropertyCount() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Properties.size();
		}
		return 0;
	}

	Map<ReflectionID, Property> Type::Properties() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Properties;
		}
		return Map<ReflectionID, Property>();
	}

	Property Type::GetProperty(const ReflectionID& ID) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto Found = TI->Properties.find(ID);
			if (Found != TI->Properties.end())
			{
				return Found->second;
			}
		}
		return Property();
	}

	Handle Type::GetPropertyValue(const Handle& Handle, const ReflectionID& ID) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto Found = TI->Properties.find(ID);
			if (Found != TI->Properties.end())
			{
				return Found->second.GetValue(Handle);
			}
		}
		return CE::Handle();
	}

	bool Type::SetPropertyValue(const Handle& Handle, const ReflectionID& ID, const Argument& Value) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto Found = TI->Properties.find(ID);
			if (Found != TI->Properties.end())
			{
				return Found->second.SetValue(Handle, Value);
			}
		}
		return false;
	}

	size_t Type::EnumEntryCount() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->EnumEntries.size();
		}
		return 0;
	}

	Map<String, ReflectionEnumEntry> Type::EnumEntries() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->EnumEntries;
		}
		return Map<String, ReflectionEnumEntry>();
	}

	String Type::NameOfEnumValue(uint64 Value) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			for (auto& Entry : TI->EnumEntries)
			{
				if (Entry.second.Value == Value)
				{
					return Entry.first;
				}
			}
		}
		return String();
	}

	uint64 Type::ValueOfEnumName(const String& Name) const
	{
		bool Success = false;
		return ValueOfEnumName(Name, Success);
	}

	uint64 Type::ValueOfEnumName(const String& Name, bool& Success) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto Found = TI->EnumEntries.find(Name);
			if (Found != TI->EnumEntries.end())
			{
				Success = true;
				return Found->second.Value;
			}
		}
		Success = false;
		return 0;
	}

	size_t Type::MetadataCount() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Metas.size();
		}
		return 0;
	}

	Map<String, String> Type::Metadatas() const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Metas;
		}
		return Map<String, String>();
	}

	bool Type::HasMetadata(const String& Key) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			return TI->Metas.find(Key) != TI->Metas.end();
		}
		return false;
	}

	String Type::Metadata(const String& Key) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
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

	size_t Type::EnumMetadataCount(const String& Name) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto FoundEnum = TI->EnumEntries.find(Name);
			if (FoundEnum != TI->EnumEntries.end())
			{
				return FoundEnum->second.Metas.size();
			}
		}
		return 0;
	}

	Map<String, String> Type::EnumMetadatas(const String& Name) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto FoundEnum = TI->EnumEntries.find(Name);
			if (FoundEnum != TI->EnumEntries.end())
			{
				return FoundEnum->second.Metas;
			}
		}
		return Map<String, String>();
	}

	bool Type::HasEnumMetadata(const String& Name, const String& Key) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto FoundEnum = TI->EnumEntries.find(Name);
			if (FoundEnum != TI->EnumEntries.end())
			{
				return FoundEnum->second.Metas.find(Key) != FoundEnum->second.Metas.end();
			}
		}
		return false;
	}

	String Type::EnumMetadata(const String& Name, const String& Key) const
	{
		SharedPtr<Detail_Reflection::TypeInfo> TI = m_Info.Lock();
		if (TI != nullptr)
		{
			auto FoundEnum = TI->EnumEntries.find(Name);
			if (FoundEnum != TI->EnumEntries.end())
			{
				auto Found = FoundEnum->second.Metas.find(Key);
				if (Found != FoundEnum->second.Metas.end())
				{
					return Found->second;
				}
			}
		}
		return String();
	}

	bool Type::operator==(const Type& Other) const
	{
		return m_ID == Other.m_ID;
	}

	bool Type::operator!=(const Type& Other) const
	{
		return m_ID != Other.m_ID;
	}

	bool Type::operator<(const Type& Other) const
	{
		return m_ID < Other.m_ID;
	}
}
