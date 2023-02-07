#include "CardinalEngine/Framework/Core/Logging/Sinks/LogSink_Stream.h"

namespace CE
{
	LogSink_Stream::LogSink_Stream(std::ostream& Stream) : m_Stream(Stream)
	{

	}

	void LogSink_Stream::Write(ELog Category, const std::string& Msg)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex);
		m_Stream.write(Msg.data(), Msg.size());
		m_Stream.write("\n", 1);
	}

	void LogSink_Stream::Flush()
	{
		m_Stream.flush();
	}
}
