#include "CardinalEngine/Framework/Core/Profiler/ThreadProfiler.h"
#include "CardinalEngine/Framework/Core/Profiler/Profiler.h"

namespace CE
{
	ThreadProfiler::ThreadProfiler()
	{
		std::lock_guard<std::shared_mutex> Lock(Profiler::Get().m_ThreadProfilers_Mutex);
		Profiler::Get().m_ThreadProfilers.Add(this);
		Array<ThreadProfiler*>& Profilers = Profiler::Get().m_ThreadProfilers;
	}

	ThreadProfiler::~ThreadProfiler()
	{
		std::lock_guard<std::shared_mutex> Lock(Profiler::Get().m_ThreadProfilers_Mutex);
		Profiler::Get().m_ThreadProfilers.Remove(this, 1);
		Array<ThreadProfiler*>& Profilers = Profiler::Get().m_ThreadProfilers;
	}

	ThreadProfiler& ThreadProfiler::Get()
	{
		static thread_local ThreadProfiler Inst;
		return Inst;
	}

	void ThreadProfiler::SetName(const std::string& Name)
	{
		std::lock_guard<std::shared_mutex> Lock(Profiler::Get().m_ThreadProfilers_Mutex);
		m_Name = Name;
	}

	void ThreadProfiler::BeginBlock_Hierarchy(const char* BlockName)
	{
		std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
		m_Events.enqueue(ProfileEvent{ ProfileEvent::EType::Hierarchy, Profiler::Get().FrameIndex(), BlockName, Now });
	}

	void ThreadProfiler::EndBlock_Hierarchy()
	{
		std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
		m_Events.enqueue(ProfileEvent{ ProfileEvent::EType::Hierarchy, Profiler::Get().FrameIndex(), nullptr, Now });
	}

	void ThreadProfiler::BeginBlock_Performance(const char* BlockName)
	{
		std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
		m_Events.enqueue(ProfileEvent{ ProfileEvent::EType::Performance, Profiler::Get().FrameIndex(), BlockName, Now });
	}

	void ThreadProfiler::EndBlock_Performance()
	{
		std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
		m_Events.enqueue(ProfileEvent{ ProfileEvent::EType::Performance, Profiler::Get().FrameIndex(), nullptr, Now });
	}
}
