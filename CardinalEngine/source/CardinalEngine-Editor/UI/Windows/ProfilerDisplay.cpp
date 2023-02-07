#include "CardinalEngine-Editor/UI/Windows/ProfilerDisplay.h"

#include <sstream>
#include <numeric>

namespace CE
{
	void ProfilerDisplay::Draw(float DeltaTime)
	{
		ImGui::Begin("Profiler", &IsOpen);

		if (ImGui::Button(m_CachedFrame == nullptr ? "Pause" : "Continue"))
		{
			if (m_CachedFrame != nullptr)
			{
				m_CachedFrame.Reset();
			}
			else
			{
				m_CachedFrame = new ProfiledFrame(Profiler::Get().GetLastFrame());
			}
		}

		ImGui::Separator();

		const ProfiledFrame* Frame = nullptr;
		if (m_CachedFrame != nullptr)
		{
			Frame = m_CachedFrame.Get();
		}
		else
		{
			Frame = &Profiler::Get().GetLastFrame();
		}

		ImGui::Text("Frame %llu", Frame->FrameIndex);
		ImGui::Separator();

		ImGui::BeginChild("Profiling");

		if (ImGui::CollapsingHeader("Hierarchy"))
		{
			for (auto& Thread : Frame->ProfiledThreads)
			{
				std::stringstream SS;
				SS << Thread.ID;
				String HeaderName;
				if (Thread.Name.Size() != 0)
				{
					HeaderName = String::Format("%s (%s)##%s", Thread.Name.Data(), SS.str().c_str(), SS.str().c_str());
				}
				else
				{
					HeaderName = SS.str();
				}
				if (ImGui::TreeNodeEx(HeaderName.Data()))
				{
					ImGui::PopID();

					ImGui::Columns(2);
					DrawHierarchyBlocks(Thread.HierarchyBlocks);
					ImGui::Columns(1);

					ImGui::PushID(HeaderName.Data());
					ImGui::TreePop();
				}
			}
		}

		if (ImGui::CollapsingHeader("Performance"))
		{
			for (auto& PerformanceEntry : Frame->PerformanceEntries)
			{
				if (PerformanceEntry.second.Durations.Size() > 0)
				{
					if (ImGui::TreeNodeEx(PerformanceEntry.first.Data()))
					{
						ImGui::PopID();

						std::chrono::high_resolution_clock::duration AverageDuration = std::reduce(PerformanceEntry.second.Durations.begin(), PerformanceEntry.second.Durations.end()) / PerformanceEntry.second.Durations.Size();
						std::chrono::high_resolution_clock::duration MinDuration = PerformanceEntry.second.Durations[0];
						std::chrono::high_resolution_clock::duration MaxDuration = PerformanceEntry.second.Durations[0];
						for (auto& Entry : PerformanceEntry.second.Durations)
						{
							if (MinDuration > Entry)
							{
								MinDuration = Entry;
							}
							if (MaxDuration < Entry)
							{
								MaxDuration = Entry;
							}
						}


						ImGui::Columns(2);
						ImGui::Text("Call Count");
						ImGui::NextColumn();
						ImGui::Text("%llu", PerformanceEntry.second.Durations.Size());

						ImGui::NextColumn();
						ImGui::Text("Minimun");
						ImGui::NextColumn();
						ImGui::Text("%.3f ms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(MinDuration).count());

						ImGui::NextColumn();
						ImGui::Text("Maximum");
						ImGui::NextColumn();
						ImGui::Text("%.3f ms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(MaxDuration).count());

						ImGui::NextColumn();
						ImGui::Text("Average");
						ImGui::NextColumn();
						ImGui::Text("%.3f ms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(AverageDuration).count());

						ImGui::Columns(1);

						ImGui::PushID(PerformanceEntry.first.Data());
						ImGui::TreePop();
					}
				}
			}
		}

		ImGui::EndChild();

		ImGui::End();
	}

	void ProfilerDisplay::DrawHierarchyBlocks(const Array<ProfiledHierarchyBlock>& Blocks)
	{
		for (auto& Block : Blocks)
		{
			ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_SpanAvailWidth;
			if (Block.SubBlocks.Size() == 0)
			{
				Flags |= ImGuiTreeNodeFlags_Bullet;
			}

			float Ms = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(Block.Stop - Block.Start).count();
			String TimeText = String::Format("%.3f ms", Ms);

			bool TreeOpen = ImGui::TreeNodeEx(Block.BlockName.Data(), Flags);

			ImGui::NextColumn();

			ImGui::Text(TimeText.Data());
			ImGui::NextColumn();

			if (TreeOpen)
			{
				DrawHierarchyBlocks(Block.SubBlocks);
				ImGui::TreePop();
			}
		}
	}
}
