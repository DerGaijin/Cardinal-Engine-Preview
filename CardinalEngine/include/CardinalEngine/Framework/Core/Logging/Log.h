#pragma once
#include "../Definitions.h"

#include "Sinks/LogSink.h"

#include <string>
#include <ctime>
#include <shared_mutex>
#include <set>
#include <intrin.h>

namespace CE
{
	class CE_API Log
	{
	public:
		Log() = delete;
		
		// Writes a Log Entry to all Sinks and formats it
		template<typename... ARGS>
		static void Write(const char* Msg, ARGS... Args)
		{
			std::time_t Time = std::time(0);
			int RequiredSize = snprintf(nullptr, 0, Msg, Args...) + 1;	// '+ 1 for the zero terminator'
			std::string FormattedMsg = Msg;
			FormattedMsg.resize(RequiredSize);
			snprintf(FormattedMsg.data(), RequiredSize, Msg, Args...);
			WriteToSinks(SourceLocation{}, ELog::Info, FormattedMsg.data(), Time);
		}

		// Writes a Log Entry to all Sinks and formats it with Source Location and Category
		template<typename... ARGS>
		static void Write(const SourceLocation& Loc, ELog Category, const char* Msg, ARGS... Args)
		{
			std::time_t Time = std::time(0);
			int RequiredSize = snprintf(nullptr, 0, Msg, Args...) + 1;	// '+ 1 for the zero terminator'
			std::string FormattedMsg = Msg;
			FormattedMsg.resize(RequiredSize);
			snprintf(FormattedMsg.data(), RequiredSize, Msg, Args...);
			WriteToSinks(Loc, Category, FormattedMsg.data(), Time);
		}

		// Writes a Log Entry to all Sinks
		static void Write(const char* Msg);

		// Writes a Log Entry to all Sinks with Source Location and Category
		static void Write(const SourceLocation& Loc, ELog Category, const char* Msg);

		// Flushes all Log Sinks
		static void Flush();

		// Checks a value and Logs it if it is False
		template<typename... ARGS>
		static bool Check(bool Val, const SourceLocation& Loc, const char* Msg, ARGS... Args)
		{
			if (!Val)
			{
				Write(Loc, ELog::Fatal, Msg, std::forward<ARGS>(Args)...);
			}
			return !Val;
		}

		// Adds a Sink that will be used for Logging
		static void AddSink(LogSink* Sink);

		// Remove a Sink
		static void RemoveSink(LogSink* Sink);

	private:
		static void WriteToSinks(const SourceLocation& Loc, ELog Category, const char* Msg, std::time_t Time);
		
		static std::string ReplaceKeyWords(
			const SourceLocation& Loc,
			const char* CategoryStr,
			const std::string& Format,
			std::time_t Time,
			const char* Msg);

		static void ReplaceAll(std::string& Str, const std::string& ToReplace, const std::string& New);

	private:
		static std::shared_mutex s_Mutex;
		static std::set<LogSink*> s_Sinks;
	};
}

#define CE_DEBUGBREAK() (__nop(), __debugbreak())

#define CE_LOG(CATEGORY, MSG, ...) CE::Log::Write(CE::SourceLocation{ __FILE__, __LINE__, __FUNCTION__ }, CATEGORY, MSG, __VA_ARGS__);

#define CE_CHECK(CHECK) (!(CE::Log::Check((CHECK), CE::SourceLocation{ __FILE__, __LINE__, __FUNCTION__ }, #CHECK " ({FILE}, {LINE})") && (CE_DEBUGBREAK(), false)))

#define CE_ASSERT(ASSERT, MSG, ...) \
if (!(ASSERT)) \
{ \
	CE_LOG(CE::ELog::Fatal, MSG " ({FILE}, {LINE})", __VA_ARGS__); \
	CE_DEBUGBREAK(); \
}



#if CE_DEBUG

#define CE_DEBUG_LOG(MSG, ...) CE_LOG(MSG, __VA_ARGS__)
#define CE_DEBUG_CHECK(CHECK) CE_CHECK(CHECK)
#define CE_DEBUG_ASSERT(ASSERT, MSG, ...) CE_ASSERT(ASSERT, MSG, __VA_ARGS__)

#else

#define CE_DEBUG_LOG(MSG, ...)
#define CE_DEBUG_CHECK(CHECK) (!(CHECK))
#define CE_DEBUG_ASSERT(ASSERT, MSG, ...)

#endif
