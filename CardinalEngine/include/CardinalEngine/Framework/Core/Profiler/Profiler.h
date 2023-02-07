#pragma once
#include "../Definitions.h"
#include "../Containers/String.h"
#include "../Containers/Array.h"
#include "../Containers/Map.h"
#include "ThreadProfiler.h"

#include <stack>
#include <chrono>

namespace CE
{
	struct ProfiledHierarchyBlock
	{
		String BlockName;
		std::chrono::high_resolution_clock::time_point Start;
		std::chrono::high_resolution_clock::time_point Stop;
		Array<ProfiledHierarchyBlock> SubBlocks;
	};

	struct ProfiledPerformanceEntry
	{
		Array<std::chrono::high_resolution_clock::duration> Durations;
	};

	struct ProfiledThread
	{
		std::thread::id ID;
		String Name;
		Array<ProfiledHierarchyBlock> HierarchyBlocks;
	};

	struct ProfiledFrame
	{
		size_t FrameIndex = 0;
		std::chrono::high_resolution_clock::time_point Start;
		std::chrono::high_resolution_clock::time_point Stop;
		Array<ProfiledThread> ProfiledThreads;
		Map<String, ProfiledPerformanceEntry> PerformanceEntries;
	};

	class CE_API Profiler
	{
		friend class ThreadProfiler;

		// Private Constructor
		Profiler() = default;
	public:
		// Returns the Profiler Singleton
		static Profiler& Get();

		// Processes the Last Frame
		void ProcessFrame();

		// Returns the Last Processed Frame
		const ProfiledFrame& GetLastFrame() const
		{
			return m_LastProfiledFrame;
		}

		// Returns the Current Frame that is Profiled
		size_t FrameIndex() const
		{
			return m_FrameIndex.load();
		}

		// Resets the Profiler (Only call if all Threads finished all blocks)
		void Reset();

	private:
		struct ProcessingData
		{
			ProfileEvent CachedEvent;
			std::stack<ProfiledHierarchyBlock*> HierarchyBlockStack;
			std::stack<std::pair<String, std::chrono::high_resolution_clock::time_point>> PerformanceBlockStack;
		};

		void ProcessThread(size_t LastFrameIndex, ProfiledThread& Data, ThreadProfiler& Profiler);

		void ProcessEvent(ProfileEvent& Event, ProcessingData& Data, ProfiledThread& Thread);

		void ClearFinsihedBlocks(Array<ProfiledHierarchyBlock>& Blocks);

	private:
		std::shared_mutex m_ThreadProfilers_Mutex;
		Array<ThreadProfiler*> m_ThreadProfilers;

		std::atomic<size_t> m_FrameIndex = 0;
		ProfiledFrame m_LastProfiledFrame;

		Map<std::thread::id, ProcessingData> m_ProcessingData;
	};
}
