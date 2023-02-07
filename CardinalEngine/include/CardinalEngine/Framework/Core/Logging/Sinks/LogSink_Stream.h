#pragma once
#include "../../Definitions.h"
#include "LogSink.h"

#include <mutex>
#include <ostream>

namespace CE
{
	class CE_API LogSink_Stream : public LogSink
	{
	public:
		LogSink_Stream(std::ostream& Stream);

	protected:
		virtual void Write(ELog Category, const std::string& Msg) override;

		virtual void Flush() override;

	private:
		std::mutex m_Mutex;
		std::ostream& m_Stream;
	};
}
