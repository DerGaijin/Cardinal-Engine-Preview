#pragma once
#include "../../Definitions.h"
#include "LogSink.h"

namespace CE
{
	class CE_API LogSink_Debugger : public LogSink
	{
	public:

	protected:
		virtual void Write(ELog Category, const std::string& Msg) override;

	};
}
