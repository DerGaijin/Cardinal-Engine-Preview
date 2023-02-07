#pragma once
#include "../../Core/Headers.h"
#include "MethodInfo.h"
#include <array>

namespace CE
{
	namespace Detail_Reflection
	{
		// Helper Function to get the ReflectionID of a Argument at Index
		template<typename Fn, size_t... S>
		std::enable_if_t<(FunctionTrait<Fn>::ArgCount != 0), ReflectionID> GetArgumentID(std::index_sequence<S...> Seq, size_t Idx)
		{
			std::array<ReflectionID, FunctionTrait<Fn>::ArgCount> ArgArray = 
			{ 
				{ReflectionID::Create<ParamTypes_t<Fn, S>>() ...}
			};
			if (Idx >= ArgArray.size())
			{
				return ReflectionID();
			}
			else
			{
				return ArgArray[Idx];
			}
		}

		// Helper Function to get the ReflectionID of a Argument at Index if the Method has no Arguments
		template<typename Fn, size_t... S>
		std::enable_if_t<(FunctionTrait<Fn>::ArgCount == 0), ReflectionID> GetArgumentID(std::index_sequence<S...> Seq, size_t Idx)
		{
			return ReflectionID();
		}



		struct MWT_StaticNoReturn {};
		struct MWT_StaticWithReturn {};
		struct MWT_MemberNoReturn {};
		struct MWT_MemberWithReturn {};

		template<typename MWT, typename Fn, typename... OTHER>
		class MethodWrapper;


		// Method Wrapper for Static Methods with void return
		template<typename Fn>
		class MethodWrapper<MWT_StaticNoReturn, Fn> : public MethodWrapperBase
		{
			using Trait = FunctionTrait<Fn>;

		public:
			MethodWrapper(Fn Func) : m_Func(Func) {}

			virtual bool IsStatic() const override
			{
				return true;
			}

			virtual size_t ArgumentCount() const override
			{
				return Trait::ArgCount;
			}

			virtual ReflectionID GetArgumentReflectionID(size_t Index) const override
			{
				return GetArgumentID<Fn>(std::make_index_sequence<Trait::ArgCount>(), Index);
			}

			virtual ReflectionID GetReturnReflectionID() const override
			{
				return ReflectionID::Create<typename Trait::ReturnType>();
			}

			virtual Any Execute(const Handle& Handle, const Array<Argument>& Arguments, bool& Result) const override
			{
				if (!DoArgumentsMatch(Arguments))
				{
					Result = false;
					return Any();
				}
				Result = true;
				InvokeImpl(std::make_index_sequence<Trait::ArgCount>(), Arguments);
				return Any();
			}

		private:
			template<size_t... Idx>
			void InvokeImpl(std::index_sequence<Idx...>, const Array<Argument>& Arguments) const
			{
				(m_Func)(Arguments[Idx].template Get<ParamTypes_t<Fn, Idx>>()...);
			}

		private:
			Fn m_Func;
		};

		// Method Wrapper for Static Methods with a return type
		template<typename Fn>
		class MethodWrapper<MWT_StaticWithReturn, Fn> : public MethodWrapperBase
		{
			using Trait = FunctionTrait<Fn>;

		public:
			MethodWrapper(Fn Func) : m_Func(Func) {}

			virtual bool IsStatic() const override
			{
				return true;
			}

			virtual size_t ArgumentCount() const override
			{
				return Trait::ArgCount;
			}

			virtual ReflectionID GetArgumentReflectionID(size_t Index) const override
			{
				return GetArgumentID<Fn>(std::make_index_sequence<Trait::ArgCount>(), Index);
			}

			virtual ReflectionID GetReturnReflectionID() const override
			{
				return ReflectionID::Create<typename Trait::ReturnType>();
			}

			virtual Any Execute(const Handle& Handle, const Array<Argument>& Arguments, bool& Result) const override
			{
				if (!DoArgumentsMatch(Arguments))
				{
					Result = false;
					return Any();
				}
				Result = true;
				return InvokeImpl(std::make_index_sequence<Trait::ArgCount>(), Arguments);
			}

		private:
			template<size_t... Idx>
			Any InvokeImpl(std::index_sequence<Idx...>, const Array<Argument>& Arguments) const
			{
				return Any::Create<typename Trait::ReturnType>((m_Func)(Arguments[Idx].template Get<ParamTypes_t<Fn, Idx>>()...));
			}

		private:
			Fn m_Func;
		};

		// Method Wrapper for Member Methods with void return
		template<typename Fn, typename O>
		class MethodWrapper<MWT_MemberNoReturn, Fn(O::*)> : public MethodWrapperBase
		{
			using Trait = FunctionTrait<Fn(O::*)>;

		public:
			MethodWrapper(Fn(O::* Func)) : m_Func(Func) {}

			virtual bool IsStatic() const override
			{
				return false;
			}

			virtual size_t ArgumentCount() const override
			{
				return Trait::ArgCount;
			}

			virtual ReflectionID GetArgumentReflectionID(size_t Index) const override
			{
				return GetArgumentID<Fn(O::*)>(std::make_index_sequence<Trait::ArgCount>(), Index);
			}

			virtual ReflectionID GetReturnReflectionID() const override
			{
				return ReflectionID::Create<typename Trait::ReturnType>();
			}

			virtual Any Execute(const Handle& Handle, const Array<Argument>& Arguments, bool& Result) const override
			{
				O* Owner = Handle.Get<O>();
				if (Owner == nullptr || !DoArgumentsMatch(Arguments))
				{
					Result = false;
					return Any();
				}
				Result = true;
				InvokeImpl(std::make_index_sequence<Trait::ArgCount>(), Owner, Arguments);
				return Any();
			}

		private:
			template<size_t... Idx>
			void InvokeImpl(std::index_sequence<Idx...>, O* Owner, const Array<Argument>& Arguments) const
			{
				(Owner->*m_Func)(Arguments[Idx].template Get<ParamTypes_t<Fn, Idx>>()...);
			}

		private:
			Fn(O::* m_Func);
		};

		// Method Wrapper for Member Methods with a return type
		template<typename Fn, typename O>
		class MethodWrapper<MWT_MemberWithReturn, Fn(O::*)> : public MethodWrapperBase
		{
			using Trait = FunctionTrait<Fn(O::*)>;

		public:
			MethodWrapper(Fn(O::* Func)) : m_Func(Func) {}

			bool IsStatic() const override
			{
				return false;
			}

			virtual size_t ArgumentCount() const override
			{
				return Trait::ArgCount;
			}

			virtual ReflectionID GetArgumentReflectionID(size_t Index) const override
			{
				return GetArgumentID<Fn(O::*)>(std::make_index_sequence<Trait::ArgCount>(), Index);
			}

			virtual ReflectionID GetReturnReflectionID() const override
			{
				return ReflectionID::Create<typename Trait::ReturnType>();
			}

			virtual Any Execute(const Handle& Handle, const Array<Argument>& Arguments, bool& Result) const override
			{
				O* Owner = Handle.Get<O>();
				if (Owner == nullptr || !DoArgumentsMatch(Arguments))
				{
					Result = false;
					return Any();
				}
				Result = true;
				return InvokeImpl(std::make_index_sequence<Trait::ArgCount>(), Owner, Arguments);
			}

		private:
			template<size_t... Idx>
			Any InvokeImpl(std::index_sequence<Idx...>, O* Owner, const Array<Argument>& Arguments) const
			{
				return Any::Create<typename Trait::ReturnType>((Owner->*m_Func)(Arguments[Idx].template Get<ParamTypes_t<Fn, Idx>>()...));
			}

		private:
			Fn(O::* m_Func);
		};



		// Helper Function to create a Method Wrapper from a Static Function Pointer
		template<typename Fn>
		UniquePtr<MethodWrapperBase> CreateMethodWrapper(Fn Func)
		{
			return new MethodWrapper<std::conditional_t<std::is_same_v<typename FunctionTrait<Fn>::ReturnType, void>, MWT_StaticNoReturn, MWT_StaticWithReturn>, Fn>(Func);
		}

		// Helper Function to create a Method Wrapper from a Member Function Pointer
		template<typename Fn, typename O>
		UniquePtr<MethodWrapperBase> CreateMethodWrapper(Fn(O::* Func))
		{
			return new MethodWrapper<std::conditional_t<std::is_same_v<typename FunctionTrait<Fn(O::*)>::ReturnType, void>, MWT_MemberNoReturn, MWT_MemberWithReturn>, Fn(O::*)>(Func);
		}
	}
}
