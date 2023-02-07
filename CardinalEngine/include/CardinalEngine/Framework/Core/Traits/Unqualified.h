#pragma once
#include "../Definitions.h"

#include <type_traits>

namespace CE
{
	template<typename T, typename Enable = void>
	struct Unqualified
	{
		using type = std::remove_cv_t<T>;
	};

	template<typename T>
	struct Unqualified<T, std::enable_if_t<std::is_pointer<T>::value && !(std::is_pointer<T>::value&& std::is_function<std::remove_pointer_t<T>>::value)>>
	{
		using type = std::add_pointer_t<typename Unqualified< std::remove_pointer_t<T>>::type>;
	};

	template<typename T>
	struct Unqualified<T, std::enable_if_t<std::is_lvalue_reference<T>::value>>
	{
		using type = std::add_lvalue_reference_t<typename Unqualified< std::remove_reference_t<T> >::type>;
	};

	template<typename T>
	struct Unqualified<T, std::enable_if_t<std::is_rvalue_reference<T>::value>>
	{
		using type = std::add_rvalue_reference_t<typename Unqualified< std::remove_reference_t<T> >::type>;
	};

	template<typename T>
	using Unqualified_t = typename Unqualified<T>::type;
}
