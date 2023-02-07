#pragma once
#include "../Containers/String.h"
#include <exception>

namespace CE
{
	// Base class for all Exceptions in Cardinal Engine
	class CardinalException : public std::exception
	{
	public:
		CardinalException() = default;
	
		CardinalException(const String& What) : m_What(What), std::exception(m_What.Data()) {}

	private:
		String m_What;
	};
}
