#pragma once
#include "../Core/Headers.h"
#include "ReflectionID.h"
#include "Detail/TypeInfo.h"
#include "Detail/MethodInfo.h"
#include "Detail/PropertyInfo.h"

namespace CE
{
	class CE_API ReflectionModule
	{
		friend class ReflectionManager;

	public:
		// Constructor
		ReflectionModule();

		// Registers a Type
		template<typename T>
		std::enable_if_t<!std::is_void_v<T>, SharedPtr<Detail_Reflection::TypeInfo>> RegisterType()
		{
			using UnqualifiedT = Unqualified_t<T>;
			static_assert(!std::is_same_v<Type, T>, "Type cannot be reflected");
			static_assert(!std::is_same_v<Method, T>, "Method cannot be reflected");
			static_assert(!std::is_same_v<Property, T>, "Property cannot be reflected");
			static_assert(!std::is_same_v<Any, T>, "Any cannot be reflected");
			static_assert(!std::is_same_v<Argument, T>, "Argument cannot be reflected");
			static_assert(!std::is_same_v<Handle, T>, "Handle cannot be reflected");

			ETypeCategory Category = ETypeCategory::Void;
			if (std::is_fundamental_v<UnqualifiedT>)
			{
				Category = ETypeCategory::Fundamental;
			}
			else if (std::is_class_v<UnqualifiedT>)
			{
				Category = ETypeCategory::Class;
			}
			else if (std::is_enum_v<UnqualifiedT>)
			{
				Category = ETypeCategory::Enum;
			}
			else if (std::is_pointer_v<UnqualifiedT>)
			{
				Category = ETypeCategory::Pointer;
			}
			else if (std::is_reference_v<UnqualifiedT>)
			{
				Category = ETypeCategory::Reference;
			}
			else
			{
				CE_CHECK(false);
			}

			return RegisterTypeImpl(ReflectionID::TypeName<UnqualifiedT>(), Category, sizeof(UnqualifiedT), GetUnderlyingID<UnqualifiedT>());
		}

		// Registers a Void Type
		template<typename T>
		std::enable_if_t<std::is_void_v<T>, SharedPtr<Detail_Reflection::TypeInfo>> RegisterType()
		{
			return RegisterTypeImpl(ReflectionID::TypeName<T>(), ETypeCategory::Void, 0, ReflectionID());
		}

		// Adds a Base to the Type Info
		void AddBase(SharedPtr<Detail_Reflection::TypeInfo> Info, const ReflectionID& BaseID);

		// Adds a Constructor to the Type Info
		SharedPtr<Detail_Reflection::MethodInfo> AddConstructor(SharedPtr<Detail_Reflection::TypeInfo> Info, UniquePtr<Detail_Reflection::MethodWrapperBase>&& Wrapper);

		// Adds a Method to the Type Info
		SharedPtr<Detail_Reflection::MethodInfo> AddMethod(SharedPtr<Detail_Reflection::TypeInfo> Info, const String& Name, UniquePtr<Detail_Reflection::MethodWrapperBase>&& Wrapper);

		// Adds a Property to the Type Info
		SharedPtr<Detail_Reflection::PropertyInfo> AddProperty(SharedPtr<Detail_Reflection::TypeInfo> Info, const String& Name, UniquePtr<Detail_Reflection::PropertyWrapperBase>&& Wrapper);

		// Registers a Static Global Method
		SharedPtr<Detail_Reflection::MethodInfo> RegisterMethod(const String& Name, UniquePtr<Detail_Reflection::MethodWrapperBase>&& Wrapper);

		// Registers a Static Global Property
		SharedPtr<Detail_Reflection::PropertyInfo> RegisterProperty(const String& Name, UniquePtr<Detail_Reflection::PropertyWrapperBase>&& Wrapper);

		// Adds a Enum Value to the Type Info
		template<typename E>
		void AddEnumValue(SharedPtr<Detail_Reflection::TypeInfo> Info, const String& Name, E Value)
		{
			AddEnumValue(Info, Name, (uint64)Value);
		}

		// Adds a Enum Value to the Type Info
		void AddEnumValue(SharedPtr<Detail_Reflection::TypeInfo> Info, const String& Name, uint64 Value);

		// Adds Metadata to the Type Info
		void AddMetadata(SharedPtr<Detail_Reflection::TypeInfo> Info, const String& Key, const String& Value);

		// Adds Metadata to the Method Info
		void AddMetadata(SharedPtr<Detail_Reflection::MethodInfo> Info, const String& Key, const String& Value);

		// Adds Metadata to the Property Info
		void AddMetadata(SharedPtr<Detail_Reflection::PropertyInfo> Info, const String& Key, const String& Value);

	private:
		template<typename T>
		static std::enable_if_t<!std::is_pointer_v<T> && !std::is_reference_v<T> && !std::is_enum_v<T>, ReflectionID> GetUnderlyingID()
		{
			return ReflectionID();
		}

		template<typename T>
		static std::enable_if_t<std::is_pointer_v<T>, ReflectionID> GetUnderlyingID()
		{
			return ReflectionID::Create<std::remove_pointer_t<T>>();
		}

		template<typename T>
		static std::enable_if_t<std::is_reference_v<T>, ReflectionID> GetUnderlyingID()
		{
			return ReflectionID::Create<std::remove_reference_t<T>>();
		}

		template<typename T>
		static std::enable_if_t<std::is_enum_v<T>, ReflectionID> GetUnderlyingID()
		{
			return ReflectionID::Create<std::underlying_type_t<T>>();
		}

		SharedPtr<Detail_Reflection::TypeInfo> RegisterTypeImpl(const String& TypeName, ETypeCategory Category, size_t Size, const ReflectionID& Underlying);

		SharedPtr<Detail_Reflection::TypeInfo> FindType(const ReflectionID& ID) const;

	private:
		bool m_IsEnabled = false;
		Map<ReflectionID, SharedPtr<Detail_Reflection::TypeInfo>> m_TypeInfos;
		SharedPtr<Detail_Reflection::TypeInfo> m_GlobalsTypeInfo;

		Array<SharedPtr<Detail_Reflection::MethodInfo>> m_MethodInfos;
		Array<SharedPtr<Detail_Reflection::PropertyInfo>> m_PropertyInfos;
	};
}
