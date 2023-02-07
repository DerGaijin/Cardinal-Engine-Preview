#pragma once
#include "../../../CardinalEngine/Framework/Core/Headers.h"
#include "../EditorWindow.h"

namespace CE
{
	class CE_API EditorLog : public EditorWindow, public LogSink
	{
	public:
		EditorLog();

		~EditorLog();

		virtual void Draw(float DeltaTime) override;

	protected:
		virtual void Write(ELog Category, const std::string& Msg) override;

	private:
		struct LogEntry
		{
			ELog Type = ELog::Info;
			String Text;
		};

		std::atomic<bool> m_Stopped = false;

		std::shared_mutex m_Mutex;
		Array<LogEntry> m_LogEntries;
		size_t NextIndex = 0;
		std::atomic<size_t> m_Count = 0;

	};
}
