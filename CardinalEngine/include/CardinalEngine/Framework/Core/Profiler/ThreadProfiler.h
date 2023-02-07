#pragma once
#include "../Definitions.h"
#include "ScopeProfiler.h"
#include "../Containers/ConcurrentQueue.h"

#include <string>

namespace CE
{
	struct ProfileEvent
	{
		enum class EType
		{
			Hierarchy,
			Performance,
		} Type = EType::Hierarchy;
		size_t FrameIndex = 0;
		const char* BlockName = nullptr;
		std::chrono::high_resolution_clock::time_point Time;
	};

	class CE_API ThreadProfiler : public Singleton
	{
		friend class Profiler;

		// Private Constructor
		ThreadProfiler();
	public:
		// Destructor
		~ThreadProfiler();

		// Returns the ThreadProfiler Singleton for each Thread
		static ThreadProfiler& Get();

	public:
		// Sets the Name of the Profiler
		void SetName(const std::string& Name);

		// Begins a Hierarchy Profiling Block
		void BeginBlock_Hierarchy(const char* BlockName);
		
		// Ends a Hierarchy Profiling Block
		void EndBlock_Hierarchy();

		// Begins a Performance Profiling Block
		void BeginBlock_Performance(const char* BlockName);
		
		// Ends a Performance Profiling Block
		void EndBlock_Performance();

	public:
		const std::thread::id ThreadID = std::this_thread::get_id();

	private:
		std::string m_Name;
		ConcurrentQueue<ProfileEvent> m_Events;
	};
}

#if CE_ENABLE_PROFILING

#define CE_PROFILE_THREAD(THREAD_NAME) CE::ThreadProfiler::Get().SetName(THREAD_NAME);

#define CE_PROFILE_BEGIN_HIERARCHY(BLOCK_NAME) CE::ThreadProfiler::Get().BeginBlock_Hierarchy(BLOCK_NAME);
#define CE_PROFILE_END_HIERARCHY() CE::ThreadProfiler::Get().EndBlock_Hierarchy();
#define CE_PROFILE_SCOPE_HIERARCHY(SCOPE_NAME) CE::ScopeProfiler CE_CONCAT(ScopeProfiler_, __LINE__)(SCOPE_NAME, false);
#define CE_PROFILE_FUNCTION_HIERARCHY() CE::ScopeProfiler FunctionProfiler_H(__FUNCTION__, false);

#define CE_PROFILE_BEGIN_PERFORMANCE(BLOCK_NAME) CE::ThreadProfiler::Get().BeginBlock_Performance(BLOCK_NAME);
#define CE_PROFILE_END_PERFORMANCE() CE::ThreadProfiler::Get().EndBlock_Performance();
#define CE_PROFILE_SCOPE_PERFORMANCE(SCOPE_NAME) CE::ScopeProfiler CE_CONCAT(ScopeProfiler_, __LINE__)(SCOPE_NAME, true);
#define CE_PROFILE_FUNCTION_PERFORMANCE() CE::ScopeProfiler FunctionProfiler_H(__FUNCTION__, true);

#else

#define CE_PROFILE_THREAD(THREAD_NAME)

#define CE_PROFILE_BEGIN_HIERARCHY(BLOCK_NAME)
#define CE_PROFILE_END_HIERARCHY()
#define CE_PROFILE_SCOPE_HIERARCHY(SCOPE_NAME)
#define CE_PROFILE_FUNCTION_HIERARCHY()

#define CE_PROFILE_BEGIN_PERFORMANCE(BLOCK_NAME)
#define CE_PROFILE_END_PERFORMANCE()
#define CE_PROFILE_SCOPE_PERFORMANCE(SCOPE_NAME)
#define CE_PROFILE_FUNCTION_PERFORMANCE()

#endif
