#include "CardinalEngine-Editor/UI/Windows/EditorLog.h"

namespace CE
{
	EditorLog::EditorLog()
	{
		m_LogEntries.AddDefaulted(20000);
		Format = "{MSG}";
		Log::AddSink(this);
	}

	EditorLog::~EditorLog()
	{
		Log::RemoveSink(this);
	}

	void EditorLog::Draw(float DeltaTime)
	{
		String WindowTitle = String::Format("Log [%llu]###EditorLog", m_Count.load());
		ImGui::Begin(WindowTitle.Data(), &IsOpen);
		if (ImGui::Button("Clear"))
		{
			std::lock_guard<std::shared_mutex> Lock(m_Mutex);
			for (auto& Entry : m_LogEntries)
			{
				Entry.Type = ELog::Info;
				Entry.Text.Clear();
			}
			NextIndex = 0;
			m_Count = 0;
		}

		ImGui::SameLine();

		if (ImGui::Button(m_Stopped.load() ? "Continue" : "  Stop  "))
		{
			m_Stopped.store(!m_Stopped.load());
		}

		ImGui::Separator();

		ImGui::BeginChild("LogText", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		{
			ImGuiListClipper clipper;
			int Offset = (int)NextIndex;

			std::shared_lock<std::shared_mutex> Lock(m_Mutex);
			if (m_LogEntries[NextIndex].Text.Size() == 0)
			{
				Offset = 0;
				clipper.Begin((int)NextIndex);
			}
			else
			{
				clipper.Begin((int)m_LogEntries.Size());
			}

			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
				{
					int It = i + Offset;
					if (It >= m_LogEntries.Size())
					{
						It -= (int)m_LogEntries.Size();
					}

					LogEntry& Entry = m_LogEntries[It];

					ImVec4 Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
					switch (Entry.Type)
					{
					default:
					case ELog::Info:
						Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
						break;
					case ELog::Trace:
						Color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
						break;
					case ELog::Warning:
						Color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
						break;
					case ELog::Error:
						Color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
						break;
					case ELog::Fatal:
						Color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
						break;
					}
					ImGui::PushStyleColor(ImGuiCol_Text, Color);
					ImGui::TextUnformatted(Entry.Text.Data(), Entry.Text.Data() + Entry.Text.Size());
					ImGui::PopStyleColor();

				}
			}
		}

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();

		ImGui::End();
	}
	
	void EditorLog::Write(ELog Category, const std::string& Msg)
	{
		if (m_Stopped.load())
		{
			return;
		}

		{
			std::shared_lock<std::shared_mutex> Lock(m_Mutex);

			m_LogEntries[NextIndex].Type = Category;
			m_LogEntries[NextIndex].Text = Msg;

			NextIndex++;
			if (NextIndex >= m_LogEntries.Size())
			{
				NextIndex = 0;
			}
		}

		m_Count++;
	}
}
