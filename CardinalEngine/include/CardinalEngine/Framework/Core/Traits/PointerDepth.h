#pragma once
#include "../Definitions.h"

#include <type_traits>

namespace CE
{
	template<typename T, typename Enable = void>
	struct PointerDepth
	{
		static constexpr size_t Value = 0;
	};

	template<typename T>
	struct PointerDepth<T, typename std::enable_if_t<std::is_pointer_v<T>>>
	{
		static constexpr size_t Value = 1 + PointerDepth<std::remove_pointer_t<T>>::Value;
	};

	template<typename T>
	static constexpr size_t PointerDepth_v = PointerDepth<T>::Value;
}
