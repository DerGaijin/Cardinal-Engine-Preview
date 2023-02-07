#pragma once
#include "../Core/Headers.h"
#include "ReflectionModule.h"
#include "Detail/MethodWrappers.h"
#include "Detail/PropertyWrappers.h"

namespace CE
{
	template<typename T>
	class ReflectionRegistrator_Type;

	template<typename C>
	class ReflectionRegistrator_Class;

	template<typename C>
	class ReflectionRegistrator_Enum;

	template<typename Fn>
	class ReflectionRegistrator_Method;

	template<typename Pr>
	class ReflectionRegistrator_Property;


	class ReflectionRegistration
	{
	public:
		ReflectionRegistration(ReflectionModule& Module) : m_Module(Module) {}

		// Registers a Type
		template<typename T>
		std::enable_if_t<!std::is_class_v<T> && !std::is_enum_v<T>, ReflectionRegistrator_Type<T>> Type()
		{
			return ReflectionRegistrator_Type<T>(m_Module);
		}

		// Registers a Class
		template<typename T>
		std::enable_if_t<std::is_class_v<T>, ReflectionRegistrator_Class<T>> Type()
		{
			return Class<T>();
		}

		// Registers a Enum
		template<typename T>
		std::enable_if_t<std::is_enum_v<T>, ReflectionRegistrator_Enum<T>> Type()
		{
			return Enum<T>();
		}

		// Registers a Class
		template<typename T>
		std::enable_if_t<std::is_class_v<T>, ReflectionRegistrator_Class<T>> Class()
		{
			return ReflectionRegistrator_Class<T>(m_Module);
		}

		// Registers a Enum
		template<typename T>
		std::enable_if_t<std::is_enum_v<T>, ReflectionRegistrator_Enum<T>> Enum()
		{
			return ReflectionRegistrator_Enum<T>(m_Module);
		}

		// Registers a Static Global Method
		template<typename Fn>
		std::enable_if_t<IsFunctionPtr<Fn>::value && !std::is_member_function_pointer_v<Fn>, ReflectionRegistrator_Method<Fn>> Method(const String& Name, Fn Func)
		{
			return ReflectionRegistrator_Method<Fn>(m_Module, m_Module.RegisterMethod(Name, Detail_Reflection::CreateMethodWrapper(Func)));
		}

		// Registers a Static Global Property
		template<typename Pr>
		std::enable_if_t<!std::is_member_object_pointer_v<Pr>, ReflectionRegistrator_Property<Pr>> Property(const String& Name, Pr Prop)
		{
			return ReflectionRegistrator_Property<Pr>(m_Module, m_Module.RegisterProperty(Name, Detail_Reflection::CreatePropertyWrapper(Prop)));
		}

	private:
		ReflectionModule& m_Module;
	};



	// Reflection Registrator for All Types that are not class/struct/Enums
	template<typename T>
	class ReflectionRegistrator_Type
	{
	public:
		ReflectionRegistrator_Type(ReflectionModule& Module) : m_Module(Module)
		{
			m_TypeInfo = m_Module.RegisterType<T>();
		}

		// Registers a Constructor Method
		template<typename Fn>
		std::enable_if_t<std::is_same_v<typename FunctionTrait<Fn>::ReturnType, T>> Constructor(Fn Func)
		{
			m_Module.AddConstructor(m_TypeInfo, Detail_Reflection::CreateMethodWrapper(Func));
		}

		// Registers a Constructor
		template<typename... ARGS>
		void Constructor()
		{
			m_Module.AddConstructor(m_TypeInfo, Detail_Reflection::CreateMethodWrapper([](ARGS... Args) { return T(std::forward<ARGS>(Args)...); }));
		}

		// Registers Metadata to the Type
		void Metadata(const String& Key, const String& Value)
		{
			m_Module.AddMetadata(m_TypeInfo, Key, Value);
		}

	protected:
		ReflectionModule& m_Module;
		SharedPtr<Detail_Reflection::TypeInfo> m_TypeInfo;
	};



	// Reflection Registrator for Classes/structs
	template<typename C>
	class ReflectionRegistrator_Class : public ReflectionRegistrator_Type<C>
	{
	public:
		ReflectionRegistrator_Class(ReflectionModule& Module) : ReflectionRegistrator_Type(Module) {}

		// Registers a Base Class to this Class
		template<typename B>
		void Base()
		{
			static_assert(std::is_base_of_v<B, C>, "B is not a base of C");
			static_assert(Detail_Reflection::HasReflectionBody<C>::value, "Type is missing CE_REFLECTION_BODY");
			static_assert(Detail_Reflection::HasReflectionBody<B>::value, "Base is missing CE_REFLECTION_BODY");

			m_Module.AddBase(m_TypeInfo, ReflectionID::Create<B>());
		}

		// Registers a Member Function to this Class
		template<typename Fn, typename O>
		ReflectionRegistrator_Method<Fn(O::*)> Method(const String& Name, Fn(O::* Func))
		{
			static_assert(std::is_same_v<C, O>, "Only register Methods of the exact Type");

			return ReflectionRegistrator_Method<Fn(O::*)>(m_Module, m_Module.AddMethod(m_TypeInfo, Name, Detail_Reflection::CreateMethodWrapper(Func)));
		}

		// Registers a Static Function to this Class
		template<typename Fn>
		ReflectionRegistrator_Method<Fn> Method(const String& Name, Fn Func)
		{
			return ReflectionRegistrator_Method<Fn>(m_Module, m_Module.AddMethod(m_TypeInfo, Name, Detail_Reflection::CreateMethodWrapper(Func)));
		}

		// Registers a Member Property to this Class
		template<typename Pr, typename O>
		ReflectionRegistrator_Property<Pr(O::*)> Property(const String& Name, Pr(O::* Prop))
		{
			static_assert(std::is_same_v<C, O>, "Only register Properties of the exact Type");

			return ReflectionRegistrator_Property<Pr(O::*)>(m_Module, m_Module.AddProperty(m_TypeInfo, Name, Detail_Reflection::CreatePropertyWrapper(Prop)));
		}

		// Registers a Static Property to this Class
		template<typename Pr>
		ReflectionRegistrator_Property<Pr> Property(const String& Name, Pr Prop)
		{
			return ReflectionRegistrator_Property<Pr>(m_Module, m_Module.AddProperty(m_TypeInfo, Name, Detail_Reflection::CreatePropertyWrapper(Prop)));
		}
	};



	// Reflection Registrator for Enums
	template<typename E>
	class ReflectionRegistrator_Enum : public ReflectionRegistrator_Type<E>
	{
	public:
		ReflectionRegistrator_Enum(ReflectionModule& Module) : ReflectionRegistrator_Type(Module) {}

		// Registers a Enum Value
		void Value(const String& Name, E Value)
		{
			m_Module.AddEnumValue(m_TypeInfo, Name, (uint64)Value);
		}
	};



	// Reflection Registrator for Methods
	template<typename Fn>
	class ReflectionRegistrator_Method
	{
	public:
		ReflectionRegistrator_Method(ReflectionModule& Module, SharedPtr<Detail_Reflection::MethodInfo> Info) : m_Module(Module), m_MethodInfo(Info) {}

		// Registers Metadata to the Property
		void Metadata(const String& Key, const String& Value)
		{
			m_Module.AddMetadata(m_MethodInfo, Key, Value);
		}

	private:
		ReflectionModule& m_Module;
		SharedPtr<Detail_Reflection::MethodInfo> m_MethodInfo;
	};



	// Reflection Registrator for Properties
	template<typename Pr>
	class ReflectionRegistrator_Property
	{
	public:
		ReflectionRegistrator_Property(ReflectionModule& Module, SharedPtr<Detail_Reflection::PropertyInfo> Info) : m_Module(Module), m_PropertyInfo(Info) {}

		// Registers Metadata to the Method
		void Metadata(const String& Key, const String& Value)
		{
			m_Module.AddMetadata(m_PropertyInfo, Key, Value);
		}

	private:
		ReflectionModule& m_Module;
		SharedPtr<Detail_Reflection::PropertyInfo> m_PropertyInfo;
	};
}
