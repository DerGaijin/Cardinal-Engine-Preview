#include "CardinalEngine/Framework/Core/Profiler/ScopeProfiler.h"
#include "CardinalEngine/Framework/Core/Profiler/ThreadProfiler.h"

namespace CE
{
	ScopeProfiler::ScopeProfiler(const char* ScopeName, bool IsPerformance) : IsPerformance(IsPerformance)
	{
		if (IsPerformance)
		{
			ThreadProfiler::Get().BeginBlock_Performance(ScopeName);
		}
		else
		{
			ThreadProfiler::Get().BeginBlock_Hierarchy(ScopeName);
		}
	}

	ScopeProfiler::~ScopeProfiler()
	{
		if (IsPerformance)
		{
			ThreadProfiler::Get().EndBlock_Performance();
		}
		else
		{
			ThreadProfiler::Get().EndBlock_Hierarchy();
		}
	}
}
