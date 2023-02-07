#include "CardinalEngine/Framework/Core/Profiler/Profiler.h"

namespace CE
{
	Profiler& Profiler::Get()
	{
		static Profiler Inst;
		return Inst;
	}

	void Profiler::ProcessFrame()
	{
		size_t LastFrameIndex = m_FrameIndex.load();
		m_FrameIndex++;
		std::shared_lock<std::shared_mutex> Lock(m_ThreadProfilers_Mutex);

		m_LastProfiledFrame.FrameIndex = LastFrameIndex;
		for (size_t i = 0; i < m_LastProfiledFrame.ProfiledThreads.Size();)
		{
			std::thread::id ThreadID = m_LastProfiledFrame.ProfiledThreads[i].ID;

			size_t FoundAt = 0;
			if (!m_ThreadProfilers.FindByPredicate([&](const ThreadProfiler* It) { return It->ThreadID == ThreadID; }, FoundAt))
			{
				m_LastProfiledFrame.ProfiledThreads.RemoveAt(i);
			}
			else
			{
				i++;
			}
		}

		for (auto It = m_LastProfiledFrame.PerformanceEntries.begin(); It != m_LastProfiledFrame.PerformanceEntries.end();)
		{
			if (It->second.Durations.Size() == 0)
			{
				It = m_LastProfiledFrame.PerformanceEntries.erase(It);
			}
			else
			{
				It->second.Durations.Clear();
			}
		}

		for (auto& ThreadProfiler : m_ThreadProfilers)
		{
			ProfiledThread* ThreadData = nullptr;
			size_t FoundAt = 0;
			if (m_LastProfiledFrame.ProfiledThreads.FindByPredicate([&](const ProfiledThread& It) { return It.ID == ThreadProfiler->ThreadID; }, FoundAt))
			{
				ThreadData = &m_LastProfiledFrame.ProfiledThreads[FoundAt];
			}
			else
			{
				ThreadData = &m_LastProfiledFrame.ProfiledThreads.EmplaceRef();
				ThreadData->ID = ThreadProfiler->ThreadID;
			}

			ThreadData->Name = ThreadProfiler->m_Name;

			ProcessThread(LastFrameIndex, *ThreadData, *ThreadProfiler);
		}
	}


	void Profiler::Reset()
	{
		std::lock_guard<std::shared_mutex> Lock(m_ThreadProfilers_Mutex);

		m_FrameIndex = 0;
		m_LastProfiledFrame = ProfiledFrame();
		m_ProcessingData.clear();

		for (auto& ThreadProfiler : m_ThreadProfilers)
		{
			ProfileEvent Event;
			while (ThreadProfiler->m_Events.try_dequeue(Event))
			{

			}
		}
	}

	void Profiler::ProcessThread(size_t LastFrameIndex, ProfiledThread& Data, ThreadProfiler& Profiler)
	{
		auto ProfilerProcessingData = m_ProcessingData.find(Data.ID);
		if (ProfilerProcessingData == m_ProcessingData.end())
		{
			ProfilerProcessingData = m_ProcessingData.emplace(Data.ID, ProcessingData()).first;
		}

		ClearFinsihedBlocks(Data.HierarchyBlocks);

		if (ProfilerProcessingData->second.CachedEvent.Time != std::chrono::high_resolution_clock::time_point())
		{
			ProcessEvent(ProfilerProcessingData->second.CachedEvent, ProfilerProcessingData->second, Data);
			ProfilerProcessingData->second.CachedEvent.Time = std::chrono::high_resolution_clock::time_point();
		}

		while (true)
		{
			ProfileEvent Event;
			if (Profiler.m_Events.try_dequeue(Event))
			{
				if (Event.FrameIndex <= LastFrameIndex)
				{
					ProcessEvent(Event, ProfilerProcessingData->second, Data);
				}
				else
				{
					ProfilerProcessingData->second.CachedEvent = Event;
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	void Profiler::ProcessEvent(ProfileEvent& Event, ProcessingData& Data, ProfiledThread& Thread)
	{
		switch (Event.Type)
		{
		default:
		case ProfileEvent::EType::Hierarchy:
		{
			if (Event.BlockName != nullptr)
			{
				ProfiledHierarchyBlock* NewBlock = nullptr;
				if (Data.HierarchyBlockStack.empty())
				{
					NewBlock = &Thread.HierarchyBlocks.EmplaceRef();
				}
				else
				{
					NewBlock = &Data.HierarchyBlockStack.top()->SubBlocks.EmplaceRef();
				}
				Data.HierarchyBlockStack.push(NewBlock);

				NewBlock->BlockName = Event.BlockName;
				NewBlock->Start = Event.Time;
			}
			else
			{
				if (Data.HierarchyBlockStack.empty())
				{
					throw;
				}
				Data.HierarchyBlockStack.top()->Stop = Event.Time;
				Data.HierarchyBlockStack.pop();
			}
			break;
		}
		case ProfileEvent::EType::Performance:
		{
			if (Event.BlockName != nullptr)
			{
				Data.PerformanceBlockStack.push(std::pair<String, std::chrono::high_resolution_clock::time_point>(Event.BlockName, Event.Time));
			}
			else
			{
				if (Data.PerformanceBlockStack.empty())
				{
					throw;
				}
				m_LastProfiledFrame.PerformanceEntries[Data.PerformanceBlockStack.top().first].Durations.EmplaceRef(Event.Time - Data.PerformanceBlockStack.top().second);
				Data.PerformanceBlockStack.pop();
			}
			break;
		}
		}
	}

	void Profiler::ClearFinsihedBlocks(Array<ProfiledHierarchyBlock>& Blocks)
	{
		if (Blocks.Size() > 0)
		{
			Blocks.RemoveAt(0, Blocks.Size() - 1);
			if (Blocks[0].Stop != std::chrono::high_resolution_clock::time_point())
			{
				Blocks.Clear();
			}
			else
			{
				ClearFinsihedBlocks(Blocks[0].SubBlocks);
			}
		}
	}
}
