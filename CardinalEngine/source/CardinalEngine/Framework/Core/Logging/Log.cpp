#include "CardinalEngine/Framework/Core/Logging/Log.h"

#include <iostream>
#include <thread>
#include <sstream>

namespace CE
{
	std::shared_mutex Log::s_Mutex;
	std::set<LogSink*> Log::s_Sinks;

	void Log::Write(const char* Msg)
	{
		std::time_t Time = std::time(0);
		WriteToSinks(SourceLocation{}, ELog::Info, Msg, Time);
	}

	void Log::Write(const SourceLocation& Loc, ELog Category, const char* Msg)
	{
		std::time_t Time = std::time(0);
		WriteToSinks(Loc, Category, Msg, Time);
	}

	void Log::Flush()
	{
		std::shared_lock<std::shared_mutex> Lock(s_Mutex);
		for (auto& Sink : s_Sinks)
		{
			Sink->Flush();
		}
	}

	void Log::AddSink(LogSink* Sink)
	{
		if (Sink == nullptr)
		{
			return;
		}

		std::lock_guard<std::shared_mutex> Lock(s_Mutex);
		s_Sinks.emplace(Sink);
	}

	void Log::RemoveSink(LogSink* Sink)
	{
		if (Sink == nullptr)
		{
			return;
		}

		std::lock_guard<std::shared_mutex> Lock(s_Mutex);
		s_Sinks.erase(Sink);
	}

	void Log::WriteToSinks(const SourceLocation& Loc, ELog Category, const char* Msg, std::time_t Time)
	{
		const char* CategoryStr = "Info";
		switch (Category)
		{
		default:
		case CE::ELog::Info:
			CategoryStr = "Info";
			break;
		case CE::ELog::Trace:
			CategoryStr = "Trace";
			break;
		case CE::ELog::Warning:
			CategoryStr = "Warning";
			break;
		case CE::ELog::Error:
			CategoryStr = "Error";
			break;
		case CE::ELog::Fatal:
			CategoryStr = "Fatal";
			break;
		}

		std::shared_lock<std::shared_mutex> Lock(s_Mutex);
		for (auto& Sink : s_Sinks)
		{
			std::string FormattedMsg = ReplaceKeyWords(Loc, CategoryStr, Sink->Format.data(), Time, Msg);
			Sink->Write(Category, FormattedMsg);
		}
	}

	std::string Log::ReplaceKeyWords(const SourceLocation& Loc, const char* CategoryStr, const std::string& Format, std::time_t Time, const char* Msg)
	{
		std::string Result = Format;
		ReplaceAll(Result, "{MSG}", Msg);
		ReplaceAll(Result, "{CATEGORY}", CategoryStr);
		ReplaceAll(Result, "{FILE}", Loc.File);
		ReplaceAll(Result, "{LINE}", std::to_string(Loc.Line));
		ReplaceAll(Result, "{FUNCTION}", Loc.Function);

		std::stringstream SS;
		SS << std::this_thread::get_id();
		ReplaceAll(Result, "{THREADID}", SS.str());

		std::tm TmTime;
		localtime_s(&TmTime, &Time);
		auto TimeToString = [](int In) -> std::string { return In < 9 ? "0" + std::to_string(In) : std::to_string(In); };
		ReplaceAll(Result, "{YEAR}", TimeToString(TmTime.tm_year + 1900));
		ReplaceAll(Result, "{MONTH}", TimeToString(TmTime.tm_mon + 1));
		ReplaceAll(Result, "{DAY}", TimeToString(TmTime.tm_mday));
		ReplaceAll(Result, "{HOUR}", TimeToString(TmTime.tm_hour));
		ReplaceAll(Result, "{MINUTE}", TimeToString(TmTime.tm_min));
		ReplaceAll(Result, "{SECOND}", TimeToString(TmTime.tm_sec));
		//ReplaceAll(Result, "{MILLI}", "");	//@TODO: Currently not supported
		//ReplaceAll(Result, "{MICRO}", "");
		return Result;
	}

	void Log::ReplaceAll(std::string& Str, const std::string& ToReplace, const std::string& New)
	{
		size_t Index = 0;
		while ((Index = Str.find(ToReplace, Index)) != std::string::npos) {
			Str.replace(Index, ToReplace.length(), New);
			Index += New.length();
		}
	}
}
