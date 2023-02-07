#pragma once
#include "../Definitions.h"

#include <type_traits>
#include <tuple>
#include <functional>

namespace CE
{
	template<typename T>
	struct IsFunctionPtr : std::conditional_t<std::is_pointer_v<T>&& std::is_function_v<std::remove_pointer_t<T>>, std::true_type, std::false_type> {};

	template <typename T>
	struct FunctionTrait : public FunctionTrait< decltype(&T::operator()) > {};

	template<typename R, typename... Args>
	struct FunctionTrait<R(Args...)>
	{
		static constexpr size_t ArgCount = sizeof...(Args);

		using ArgTypes = std::tuple<Args...>;
		using ReturnType = R;
	};

	template<typename R, typename... Args>
	struct FunctionTrait<R(*)(Args...)> : public FunctionTrait<R(Args...)> { };

	template<typename R, typename... Args>
	struct FunctionTrait<R(&)(Args...)> : public FunctionTrait<R(Args...)> { };

	template<typename R, typename C, typename... Args>
	struct FunctionTrait<R(C::*)(Args...)> : public FunctionTrait<R(Args...)> { using OwnerType = C; };

	template<typename R, typename C, typename... Args>
	struct FunctionTrait<R(C::*)(Args...) const> : public FunctionTrait<R(Args...)> { using OwnerType = C; };

	template<typename R, typename C, typename... Args>
	struct FunctionTrait<R(C::*)(Args...) volatile> : public FunctionTrait<R(Args...)> { using OwnerType = C; };

	template<typename R, typename C, typename... Args>
	struct FunctionTrait<R(C::*)(Args...) const volatile> : public FunctionTrait<R(Args...)> { using OwnerType = C; };

	template<typename F, size_t Index>
	struct ParamTypes { using type = typename std::tuple_element<Index, typename FunctionTrait<F>::ArgTypes>::type; };

	template<typename F, size_t Index>
	using ParamTypes_t = typename ParamTypes<F, Index>::type;


	template<typename T>
	struct FunctionTrait<std::function<T>> : public FunctionTrait<T> {};
}
