#include "CardinalEngine/Framework/Core/Logging/Sinks/LogSink_Debugger.h"

#if CE_PLATFORM == CE_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace CE
{
	void LogSink_Debugger::Write(ELog Category, const std::string& Msg)
	{
		std::string MsgWithNewLine = Msg + "\n";
#if CE_PLATFORM == CE_PLATFORM_WINDOWS
		OutputDebugStringA(MsgWithNewLine.data());
#else
#error No LogSink_Debugger implementation for this Platform
#endif
	}
}
