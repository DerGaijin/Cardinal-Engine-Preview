#pragma once
#include "../Core/Headers.h"
#include "ReflectionID.h"

namespace CE
{
	class Type;

	namespace Detail_Reflection
	{
		struct TypeInfo;

		// Trait for checking if a class has the function 'CE::Type GetType() const'
		template<typename T>
		struct HasFunction_GetType
		{
		private:
			typedef char YesType[1];
			typedef char NoType[2];
			template <typename U, CE::Type(U::*)() const>
			class check { };
			template <typename C> \
				static YesType& f(check<C, &C::GetType>*);
			template <typename C>
			static NoType& f(...);
		public:
			using type = std::conditional_t<std::is_same_v<decltype(f<typename std::remove_cv_t<std::remove_reference_t<T>>>(0)), YesType&>, std::true_type, std::false_type>;
		};

		// Trait to validate if a Class has a ReflectionValidateBody Alias
		template<typename T>
		struct HasAlias_ReflectionValidateBody
		{
		private:
			typedef char YesType[1];
			typedef char NoType[2];
			template <typename U> static YesType& check(typename U::ReflectionValidateBody*);
			template <typename U> static NoType& check(...);
		public:
			static constexpr bool value = (sizeof(check<T>(0)) == sizeof(YesType));
		};

		// Trait to validate if a Class has a ReflectionBody (CE_REFLECTION_BODY)
		template <typename T, typename Enable = void>
		struct HasReflectionBody
		{
			static constexpr bool value = false;
		};

		// Trait to validate if a Class has a ReflectionBody (CE_REFLECTION_BODY)
		template <typename T>
		struct HasReflectionBody< T, typename std::enable_if_t<HasAlias_ReflectionValidateBody<T>::value>>
		{
			static constexpr bool value = std::is_same_v<typename T::ReflectionValidateBody, T>;
		};
	}

	class Any;
	class Handle;
	class Argument;
	class Method;
	class Property;

	// The Category of a Type
	enum class ETypeCategory
	{
		Void,
		Fundamental,
		Class,
		Enum,
		Pointer,
		Reference,
	};

	// Contains the Reflected information of a Enum Entry
	struct ReflectionEnumEntry
	{
		uint64 Value = 0;
		Map<String, String> Metas;
	};

	class CE_API Type
	{
		friend class ReflectionModule;

	public:
		// Returns the Type for T if it registered
		template<typename T>
		static Type Get()
		{
			static Type Cache;
			if (!Cache.IsValid())
			{
				Cache = Find(ReflectionID::Create<T>());
			}
			return Cache;
		}

		// Returns the Type of Inst
		template<typename T>
		static std::enable_if_t<Detail_Reflection::HasFunction_GetType<T>::type::value, Type> Get(const T& Inst)
		{
			return Inst.GetType();
		}

		// Returns the Type of Inst
		template<typename T>
		static std::enable_if_t<!Detail_Reflection::HasFunction_GetType<T>::type::value, Type> Get(const T& Inst)
		{
			return Type::Get<T>();
		}

		// Returns the Type of Inst
		template<typename T>
		static std::enable_if_t<Detail_Reflection::HasFunction_GetType<T>::type::value, Type> Get(T* Inst)
		{
			return Inst != nullptr ? Inst->GetType() : Type::Get<nullptr_t>();
		}

		// Returns the Type of Inst
		template<typename T>
		static std::enable_if_t<!Detail_Reflection::HasFunction_GetType<T>::type::value, Type> Get(T* Inst)
		{
			return Type::Get<T>();
		}

		// Returns the Type registered with the ID
		static Type Find(const ReflectionID& ID);

		// Returns all registered Types
		static Map<ReflectionID, Type> All();

	public:
		// Default Constructor
		Type() = default;

		// TypeInfo Constructor
		Type(const WeakPtr<Detail_Reflection::TypeInfo>& Info);

	public:
		// Returns true if the Type is valid
		bool IsValid() const;

		// Returns the ID of the Type
		const ReflectionID& ID() const;

		// Returns the TypeName
		const String& TypeName() const;

		// Returns the Category
		ETypeCategory Category() const;

		// Returns the Size of the Type
		size_t Size() const;

		// Returns the Underlying Type or Empty Type if it has no Underlying Type
		Type Underlying() const;



		// Returns the Number of Constructors
		size_t ConstructorCount() const;

		// Returns all Constructors
		Array<Method> Constructors() const;

		// Returns a Constructor with ArgumentTypes
		Method GetConstructor(const Array<Type>& ArgumentTypes = {}) const;

		// Constructs the Type if a Constructor with matching Arguments was found
		Any Construct(const Array<Argument>& Arguments) const;



		// Returns the Number of Bases
		size_t BaseCount() const;

		// Returns all Base Types
		Array<Type> Bases() const;

		// Returns true if this Type is a base of Child
		bool IsBaseOf(const Type& Child) const;

		// Returns true if this Type is a base of Child
		template<typename T>
		bool IsBaseOf() const
		{
			return IsBaseOf(Type::Get<T>());
		}

		// Returns true if this Type is a Child of Base
		bool IsChildOf(const Type& Base) const;

		// Returns true if this Type is a Child of Base
		template<typename T>
		bool IsChildOf() const
		{
			return IsChildOf(Type::Get<T>());
		}



		// Returns Number of Methods
		size_t MethodCount() const;

		// Returns all Methods
		MultiMap<ReflectionID, Method> Methods() const;

		// Returns Method registered to the ID
		Method GetMethod(const ReflectionID& ID) const;

		// Returns Method Overload registered to the ID with ArgumentTypes
		Method GetMethod(const ReflectionID& ID, const Array<Type>& ArgumentTypes) const;

		// Searches for the Method registered to the ID and executes it with Arguments
		Any ExecuteMethod(const Handle& Handle, const ReflectionID& ID, const Array<Argument>& Arguments = {}) const;

		// Searches for the Method registered to the ID and executes it with Arguments
		Any ExecuteMethod(const Handle& Handle, const ReflectionID& ID, const Array<Argument>& Arguments, bool& Result) const;



		// Returns the Number of Properties
		size_t PropertyCount() const;

		// Returns all Properties
		Map<ReflectionID, Property> Properties() const;

		// Returns the Property registered to the ID
		Property GetProperty(const ReflectionID& ID) const;

		// Returns a Handle to the Value of the Property registered to the ID
		Handle GetPropertyValue(const Handle& Handle, const ReflectionID& ID) const;

		// Sets the Value of the Property registered to the ID
		bool SetPropertyValue(const Handle& Handle, const ReflectionID& ID, const Argument& Value) const;



		// Returns the Number of Enum Entries
		size_t EnumEntryCount() const;

		// Returns all Enum Entries
		Map<String, ReflectionEnumEntry> EnumEntries() const;

		// Returns the first Name with the Value
		String NameOfEnumValue(uint64 Value) const;

		// Returns the Value of the Name
		uint64 ValueOfEnumName(const String& Name) const;

		// Returns the Value of the Name
		uint64 ValueOfEnumName(const String& Name, bool& Success) const;



		// Returns the number of Metadatas
		size_t MetadataCount() const;

		// Returns all Metadatas
		Map<String, String> Metadatas() const;

		// Returns true if Metadata contains Key
		bool HasMetadata(const String& Key) const;

		// Returns the Value of Metadata Key
		String Metadata(const String& Key) const;

		// Returns the number of Metadatas of the Enum Entry
		size_t EnumMetadataCount(const String& Name) const;

		// Returns all Metadatas of the Enum Entry
		Map<String, String> EnumMetadatas(const String& Name) const;

		// Returns true if Metadata contains Key of the Enum Entry
		bool HasEnumMetadata(const String& Name, const String& Key) const;

		// Returns the Value of Metadata Key of the Enum Entry
		String EnumMetadata(const String& Name, const String& Key) const;

	public:
		// Equal Operator
		bool operator==(const Type& Other) const;

		// Not Equal Operator
		bool operator!=(const Type& Other) const;

		// Less Operator
		bool operator<(const Type& Other) const;

	private:
		ReflectionID m_ID;
		WeakPtr<Detail_Reflection::TypeInfo> m_Info;
	};
}


#define CE_REFLECTION_BODY() \
	public: \
	virtual CE::Type GetType() const { return CE::Type::Get<std::remove_reference_t<decltype(*this)>>(); } \
	static CE::Type StaticType() { return CE::Type::Get<typename CE::FunctionTrait<decltype(&GetType)>::OwnerType>(); } \
	using ReflectionValidateBody = typename CE::FunctionTrait<decltype(&GetType)>::OwnerType;


// Any ExecuteMethod(const String& MethodName, const Array<Argument>& Arguments = {});
// Handle GetProperty(const String& PropertyName);
// bool SetProperty(const String& PropertyName, const Argument& Value);
