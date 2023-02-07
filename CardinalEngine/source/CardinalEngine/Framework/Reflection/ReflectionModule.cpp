#include "CardinalEngine/Framework/Reflection/ReflectionModule.h"
#include "CardinalEngine/Framework/Reflection/ReflectionManager.h"

namespace CE
{
	ReflectionModule::ReflectionModule()
	{
		m_GlobalsTypeInfo = SharedPtr<Detail_Reflection::TypeInfo>(new Detail_Reflection::TypeInfo{ ReflectionID(0), ETypeCategory::Void, "", 0 });
	}

	void ReflectionModule::AddBase(SharedPtr<Detail_Reflection::TypeInfo> Info, const ReflectionID& BaseID)
	{
		// Don't allow registration on enabled modules
		if (m_IsEnabled)
		{
			return;
		}

		// Check if the Base is registered
		SharedPtr<Detail_Reflection::TypeInfo> BaseInfo = FindType(BaseID);
		CE_ASSERT(BaseInfo != nullptr, "Tried to register Base to '%s' but the Base is not registered", Info->TypeName.Data());

		// Add Base
		Info->Bases.AddUnique(Type(BaseInfo));

		// Add the Bases of the Base class
		for (auto& BaseBase : BaseInfo->Bases)
		{
			Info->Bases.AddUnique(BaseBase);
		}

		// Add Methods of the Base class
		for (auto& MethodIt : BaseInfo->Methods)
		{
			SharedPtr<Detail_Reflection::MethodInfo> MI = MethodIt.second.m_Info.Lock();
			if (MI != nullptr)
			{
				const Method& Found = Method::FindMethodOverload(Info->Methods, MI->ID, MI->Wrapper->ArgumentTypes);
				CE_ASSERT(!Found.IsValid(), "Base Method '%s' already exists", MI->Name.Data());
				Info->Methods.emplace(MethodIt.first, MethodIt.second);
			}
		}

		// Add Properties of the Base class
		for (auto& PropIt : BaseInfo->Properties)
		{
			SharedPtr<Detail_Reflection::PropertyInfo> PI = PropIt.second.m_Info.Lock();
			if (PI != nullptr)
			{
				auto Found = Info->Properties.find(PI->ID);
				CE_ASSERT(Found == Info->Properties.end(), "Base Method '%s' already exists", PI->Name.Data());
				Info->Properties.emplace(PropIt.first, PropIt.second);
			}
		}
	}

	SharedPtr<Detail_Reflection::MethodInfo> ReflectionModule::AddConstructor(SharedPtr<Detail_Reflection::TypeInfo> Info, UniquePtr<Detail_Reflection::MethodWrapperBase>&& Wrapper)
	{
		// Don't allow registration on enabled modules or empty input
		if (m_IsEnabled || Info == nullptr || Wrapper == nullptr)
		{
			return nullptr;
		}

		// Check all argument types are registered and fill the ArgumenTypes array
		Wrapper->ArgumentTypes.Reserve(Wrapper->ArgumentCount());
		for (size_t i = 0; i < Wrapper->ArgumentCount(); i++)
		{
			SharedPtr<Detail_Reflection::TypeInfo> ArgType = FindType(Wrapper->GetArgumentReflectionID(i));
			CE_ASSERT(ArgType != nullptr, "Tried to register Constructor of '%s' with Arguments that are not registered", Info->TypeName.Data());
			Wrapper->ArgumentTypes.Add(Type(ArgType));
		}

		// Check that the return type is registered
		SharedPtr<Detail_Reflection::TypeInfo> RetType = FindType(Wrapper->GetReturnReflectionID());
		CE_ASSERT(RetType != nullptr, "Tried to register Constructor of '%s' with Return Type that is not registered", Info->TypeName.Data());
		CE_ASSERT(RetType == Info, "Tried to register Constructor of '%s' that does not Return expected Type", Info->TypeName.Data());
		Wrapper->ReturnType = Type(RetType);

		// Check if a Constructor with these argument Types already exists
		for (auto& Con : Info->Constructors)
		{
			SharedPtr<Detail_Reflection::MethodInfo> TI = Con.m_Info.Lock();
			if (TI != nullptr)
			{
				CE_ASSERT(!(TI->Wrapper->ArgumentTypes == Wrapper->ArgumentTypes), "Tried to regiser Contructor of '%s' that is already registerd", Info->TypeName.Data());
			}
		}

		// Add Constructor
		SharedPtr<Detail_Reflection::MethodInfo> MI = SharedPtr<Detail_Reflection::MethodInfo>(new Detail_Reflection::MethodInfo{ ReflectionID(), "", std::move(Wrapper), Type(Info) });
		Info->Constructors.Emplace(MI);
		m_MethodInfos.Emplace(MI);

		return MI;
	}

	SharedPtr<Detail_Reflection::MethodInfo> ReflectionModule::AddMethod(SharedPtr<Detail_Reflection::TypeInfo> Info, const String& Name, UniquePtr<Detail_Reflection::MethodWrapperBase>&& Wrapper)
	{
		// Don't allow registration on enabled modules or empty input
		if (m_IsEnabled || Info == nullptr || Wrapper == nullptr || Name.Size() == 0)
		{
			return nullptr;
		}

		// Check all argument types are registered and fill the ArgumenTypes array
		Wrapper->ArgumentTypes.Reserve(Wrapper->ArgumentCount());
		for (size_t i = 0; i < Wrapper->ArgumentCount(); i++)
		{
			SharedPtr<Detail_Reflection::TypeInfo> ArgType = FindType(Wrapper->GetArgumentReflectionID(i));
			CE_ASSERT(ArgType != nullptr, "Tried to register Method '%s' with Arguments that are not registered", Name.Data());
			Wrapper->ArgumentTypes.Add(Type(ArgType));
		}

		// Check that the return type is registered
		SharedPtr<Detail_Reflection::TypeInfo> RetType = FindType(Wrapper->GetReturnReflectionID());
		CE_ASSERT(RetType != nullptr, "Tried to register Method '%s' with Return Type that is not registered", Name.Data());
		Wrapper->ReturnType = Type(RetType);

		// Check if a Method with these argument Types already exists
		ReflectionID MethodID = ReflectionID(Name);
		const Method& FoundMethod = Method::FindMethodOverload(Info->Methods, MethodID, Wrapper->ArgumentTypes);
		CE_ASSERT(!FoundMethod.IsValid(), "Tried to register Method '%s' Overload that is already registered", Name.Data());

		// Add Method
		SharedPtr<Detail_Reflection::MethodInfo> MI = SharedPtr<Detail_Reflection::MethodInfo>(new Detail_Reflection::MethodInfo{ MethodID, Name, std::move(Wrapper), Type(Info) });
		Info->Methods.emplace(MethodID, MI);
		m_MethodInfos.Emplace(MI);

		return MI;
	}

	SharedPtr<Detail_Reflection::PropertyInfo> ReflectionModule::AddProperty(SharedPtr<Detail_Reflection::TypeInfo> Info, const String& Name, UniquePtr<Detail_Reflection::PropertyWrapperBase>&& Wrapper)
	{
		// Don't allow registration on enabled modules or empty input
		if (m_IsEnabled || Info == nullptr || Wrapper == nullptr || Name.Size() == 0)
		{
			return nullptr;
		}

		// Check that the Type of the Property is registered
		SharedPtr<Detail_Reflection::TypeInfo> PropType = FindType(Wrapper->TypeReflectionID());
		CE_ASSERT(PropType != nullptr, "Tried to register Property '%s' with Type that is not registered", Name.Data());
		Wrapper->PropType = Type(PropType);

		// Check if a Property with the ID already exists
		ReflectionID PropertyID = ReflectionID(Name);
		auto Found = Info->Properties.find(PropertyID);
		CE_ASSERT(Found == Info->Properties.end(), "Tried to register Property '%s' that is already registered", Name.Data());

		// Add Property
		SharedPtr<Detail_Reflection::PropertyInfo> PI = SharedPtr<Detail_Reflection::PropertyInfo>(new Detail_Reflection::PropertyInfo{ PropertyID, Name, std::move(Wrapper), Type(Info) });
		Info->Properties.emplace(PropertyID, PI);
		m_PropertyInfos.Emplace(PI);

		return PI;
	}

	SharedPtr<Detail_Reflection::MethodInfo> ReflectionModule::RegisterMethod(const String& Name, UniquePtr<Detail_Reflection::MethodWrapperBase>&& Wrapper)
	{
		return AddMethod(m_GlobalsTypeInfo, Name, std::forward<UniquePtr<Detail_Reflection::MethodWrapperBase>&&>(Wrapper));
	}

	SharedPtr<Detail_Reflection::PropertyInfo> ReflectionModule::RegisterProperty(const String& Name, UniquePtr<Detail_Reflection::PropertyWrapperBase>&& Wrapper)
	{
		return AddProperty(m_GlobalsTypeInfo, Name, std::forward<UniquePtr<Detail_Reflection::PropertyWrapperBase>&&>(Wrapper));
	}

	void ReflectionModule::AddEnumValue(SharedPtr<Detail_Reflection::TypeInfo> Info, const String& Name, uint64 Value)
	{
		// Don't allow registration on enabled modules or empty input
		if (m_IsEnabled || Name.Size() == 0)
		{
			return;
		}

		// Check if the Enum Name already exists
		auto Found = Info->EnumEntries.find(Name);
		CE_ASSERT(Found == Info->EnumEntries.end(), "Tried to add a Enum Value to '%s' with Name '%s' that already exists", Info->TypeName.Data(), Name.Data());

		// Add Enum Entry
		Info->EnumEntries.emplace(Name, ReflectionEnumEntry{ Value });
	}

	void ReflectionModule::AddMetadata(SharedPtr<Detail_Reflection::TypeInfo> Info, const String& Key, const String& Value)
	{
		// Don't allow registration on enabled modules or empty input
		if (m_IsEnabled || Key.Size() == 0 || Value.Size() == 0)
		{
			return;
		}

		// Check if Metadata Key already exists
		auto Found = Info->Metas.find(Key);
		CE_ASSERT(Found == Info->Metas.end(), "Tried to register Metadata '%s' to Type '%s' but it already exists", Key.Data(), Info->TypeName.Data());

		// Add Metadata
		Info->Metas.emplace(Key, Value);
	}

	void ReflectionModule::AddMetadata(SharedPtr<Detail_Reflection::MethodInfo> Info, const String& Key, const String& Value)
	{
		// Don't allow registration on enabled modules or empty input
		if (m_IsEnabled || Key.Size() == 0 || Value.Size() == 0)
		{
			return;
		}

		// Check if Metadata Key already exists
		auto Found = Info->Metas.find(Key);
		CE_ASSERT(Found == Info->Metas.end(), "Tried to register Metadata '%s' to Method '%s' but it already exists", Key.Data(), Info->Name.Data());

		// Add Metadata
		Info->Metas.emplace(Key, Value);
	}

	void ReflectionModule::AddMetadata(SharedPtr<Detail_Reflection::PropertyInfo> Info, const String& Key, const String& Value)
	{
		// Don't allow registration on enabled modules or empty input
		if (m_IsEnabled || Key.Size() == 0 || Value.Size() == 0)
		{
			return;
		}

		// Check if Metadata Key already exists
		auto Found = Info->Metas.find(Key);
		CE_ASSERT(Found == Info->Metas.end(), "Tried to register Metadata '%s' to Property '%s' but it already exists", Key.Data(), Info->Name.Data());

		// Add Metadata
		Info->Metas.emplace(Key, Value);
	}

	SharedPtr<Detail_Reflection::TypeInfo> ReflectionModule::RegisterTypeImpl(const String& TypeName, ETypeCategory Category, size_t Size, const ReflectionID& Underlying)
	{
		// Don't allow registration on enabled modules
		if (m_IsEnabled)
		{
			return nullptr;
		}

		// Check if the Underlying Type is registered
		SharedPtr<Detail_Reflection::TypeInfo> UnderlyingType = nullptr;
		if (Underlying.Hash() != 0)
		{
			UnderlyingType = FindType(Underlying);
			CE_ASSERT(UnderlyingType != nullptr, "Tried to register type '%s' because the underlying type is not registered", TypeName.Data());
		}

		// Add Type Registration
		ReflectionID TypeID = ReflectionID(TypeName);
		auto EmplaceResult = m_TypeInfos.emplace(TypeID, SharedPtr<Detail_Reflection::TypeInfo>(new Detail_Reflection::TypeInfo{ TypeID, Category, TypeName, Size, Type(UnderlyingType) }));
		CE_ASSERT(EmplaceResult.second, "Tried to register type '' but it is already registered", TypeName.Data());

		return EmplaceResult.first->second;
	}

	SharedPtr<Detail_Reflection::TypeInfo> ReflectionModule::FindType(const ReflectionID& ID) const
	{
		ReflectionManager& RM = ReflectionManager::Get();

		SharedPtr<Detail_Reflection::TypeInfo> Result;
		{
			std::lock_guard<std::mutex> Lock(RM.m_TypeInfosMutex);

			// Search the Reflection Manager for the Type
			auto Found = RM.m_TypesInfos.find(ID);
			if (Found != RM.m_TypesInfos.end())
			{
				Result = Found->second.m_Info.Lock();
			}
		}
		if (Result == nullptr)
		{
			// Search the Module for the Type
			auto Found = m_TypeInfos.find(ID);
			if (Found != m_TypeInfos.end())
			{
				Result = Found->second;
			}
		}

		return Result;
	}
}
