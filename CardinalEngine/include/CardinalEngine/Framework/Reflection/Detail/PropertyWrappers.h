#pragma once
#include "../../Core/Headers.h"
#include "PropertyInfo.h"

namespace CE
{
	namespace Detail_Reflection
	{
		struct PWT_Static {};
		struct PWT_StaticReadOnly {};
		struct PWT_Member {};
		struct PWT_MemberReadOnly {};


		template<typename PWT, typename Pr, typename... OTHER>
		class PropertyWrapper;


		// Property Wrapper for Static Properties
		template<typename Pr>
		class PropertyWrapper<PWT_Static, Pr> : public PropertyWrapperBase
		{
		public:
			PropertyWrapper(Pr Prop) : m_Prop(Prop) {}

			virtual ReflectionID TypeReflectionID() const override
			{
				return ReflectionID::Create<std::remove_pointer_t<Pr>>();
			}

			virtual bool IsStatic() const override
			{
				return true;
			}

			virtual bool IsReadOnly() const override
			{
				return false;
			}

			virtual Handle Get(const Handle& Handle) const override
			{
				return CE::Handle(*m_Prop);
			}

			virtual bool Set(const Handle& Handle, const Argument& Value) const override
			{
				if (!Value.IsSuitable(PropType))
				{
					return false;
				}
				(*m_Prop) = Value.Get<std::remove_pointer_t<Pr>>();
				return true;
			}

		private:
			Pr m_Prop;
		};

		// Property Wrapper for Static Read only Properties
		template<typename Pr>
		class PropertyWrapper<PWT_StaticReadOnly, Pr> : public PropertyWrapperBase
		{
		public:
			PropertyWrapper(Pr Prop) : m_Prop(Prop) {}

			virtual ReflectionID TypeReflectionID() const override
			{
				return ReflectionID::Create<std::remove_pointer_t<Pr>>();
			}

			virtual bool IsStatic() const override
			{
				return true;
			}

			virtual bool IsReadOnly() const override
			{
				return false;
			}

			virtual Handle Get(const Handle& Handle) const override
			{
				return CE::Handle(*m_Prop);
			}

			virtual bool Set(const Handle& Handle, const Argument& Value) const override
			{
				return false;
			}

		private:
			Pr m_Prop;
		};

		// Property Wrapper for Member Properties
		template<typename Pr, typename O>
		class PropertyWrapper<PWT_Member, Pr(O::*)> : public PropertyWrapperBase
		{
		public:
			PropertyWrapper(Pr(O::* Prop)) : m_Prop(Prop) {}

			virtual ReflectionID TypeReflectionID() const override
			{
				return ReflectionID::Create<Pr>();
			}

			virtual bool IsStatic() const override
			{
				return false;
			}

			virtual bool IsReadOnly() const override
			{
				return false;
			}

			virtual Handle Get(const Handle& Handle) const override
			{
				O* Owner = Handle.Get<O>();
				if (Owner == nullptr)
				{
					return CE::Handle();
				}
				return CE::Handle((Owner->*m_Prop));
			}

			virtual bool Set(const Handle& Handle, const Argument& Value) const override
			{
				O* Owner = Handle.Get<O>();
				if (Owner == nullptr || !Value.IsSuitable(PropType))
				{
					return false;
				}
				(Owner->*m_Prop) = Value.Get<Pr>();
				return true;
			}

		private:
			Pr(O::* m_Prop);
		};

		// Property Wrapper for Member Read only Properties
		template<typename Pr, typename O>
		class PropertyWrapper<PWT_MemberReadOnly, Pr(O::*)> : public PropertyWrapperBase
		{
		public:
			PropertyWrapper(Pr(O::* Prop)) : m_Prop(Prop) {}

			virtual ReflectionID TypeReflectionID() const override
			{
				return ReflectionID::Create<Pr>();
			}

			virtual bool IsStatic() const override
			{
				return false;
			}

			virtual bool IsReadOnly() const override
			{
				return false;
			}

			virtual Handle Get(const Handle& Handle) const override
			{
				O* Owner = Handle.Get<O>();
				if (Owner == nullptr)
				{
					return CE::Handle();
				}
				return CE::Handle((Owner->*m_Prop));
			}

			virtual bool Set(const Handle& Handle, const Argument& Value) const override
			{
				return false;
			}

		private:
			Pr(O::* m_Prop);
		};


		// Helper Function to create a Property Wrapper from a Static Property Pointer
		template<typename Pr>
		UniquePtr<PropertyWrapperBase> CreatePropertyWrapper(Pr Prop)
		{
			return new PropertyWrapper<std::conditional_t<std::is_const_v<std::remove_pointer_t<Pr>>, PWT_StaticReadOnly, PWT_Static>, Pr>(Prop);
		}

		// Helper Function to create a Property Wrapper from a Member Property Pointer
		template<typename Pr, typename O>
		UniquePtr<PropertyWrapperBase> CreatePropertyWrapper(Pr(O::* Prop))
		{
			return new PropertyWrapper<std::conditional_t<std::is_const_v<Pr>, PWT_MemberReadOnly, PWT_Member>, Pr(O::*)>(Prop);
		}
	}
}
