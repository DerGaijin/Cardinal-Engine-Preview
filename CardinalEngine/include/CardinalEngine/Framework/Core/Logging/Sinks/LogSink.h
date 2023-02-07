#pragma once
#include "../../Definitions.h"

#include <array>
#include <string>

namespace CE
{
	enum class ELog
	{
		Info,
		Trace,
		Warning,
		Error,
		Fatal,
	};

	struct SourceLocation
	{
		const char* File = "";
		int Line = 0;
		const char* Function = "";
	};

	class CE_API LogSink
	{
		friend class Log;
	public:
		// Destructor
		virtual ~LogSink() = default;

	protected:
		// Writes a Log Entry
		virtual void Write(ELog Category, const std::string& Msg) = 0;

		// Flushes the log
		virtual void Flush() {}

	public:
		std::string Format = "[{CATEGORY}] {MSG}";
	};
}
