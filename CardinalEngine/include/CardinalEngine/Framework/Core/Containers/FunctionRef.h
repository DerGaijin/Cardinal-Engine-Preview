#pragma once
#include "../Definitions.h"

#include <functional>

namespace CE
{
	template<typename Fn>
	using FunctionRef = std::function<Fn>;
}
