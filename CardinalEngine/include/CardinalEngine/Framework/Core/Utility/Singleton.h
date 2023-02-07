#pragma once
#include "../Definitions.h"

namespace CE
{
	class CE_API NonCopyable
	{
	protected:
		// Protected Constructor
		NonCopyable() = default;

	public:
		// Destructor
		virtual ~NonCopyable() = default;

		// Deleted Copy Constructor
		NonCopyable(const NonCopyable&) = delete;

		// Deleted Copy Operator
		NonCopyable& operator=(const NonCopyable&) = delete;
	};

	class CE_API Singleton
	{
	protected:
		// Protected Constructor
		Singleton() = default;

	public:
		// Destructor
		virtual ~Singleton() = default;

		// Deleted Copy Constructor
		Singleton(const Singleton&) = delete;

		// Deleted Move Constructor
		Singleton(Singleton&&) = delete;

		// Deleted Copy Operator
		Singleton& operator=(const Singleton&) = delete;

		// Deleted Move Operator
		Singleton& operator=(Singleton&&) = delete;
	};
}
