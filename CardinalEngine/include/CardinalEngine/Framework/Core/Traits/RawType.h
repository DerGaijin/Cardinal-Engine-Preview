#pragma once
#include "../Definitions.h"

#include <type_traits>

namespace CE
{
	template<typename T, typename Enable = void>
	struct RawType
	{
		using type = std::remove_cv_t<T>;
	};

	template<typename T>
	struct RawType<T, std::enable_if_t<std::is_pointer<T>::value && !(std::is_pointer<T>::value&& std::is_function<std::remove_pointer_t<T>>::value)>>
	{
		using type = typename RawType< std::remove_pointer_t<T>>::type;
	};

	template<typename T>
	struct RawType<T, std::enable_if_t<std::is_reference<T>::value>>
	{
		using type = typename RawType< std::remove_reference_t<T> >::type;
	};

	template<typename T>
	using RawType_t = typename RawType<T>::type;
}
