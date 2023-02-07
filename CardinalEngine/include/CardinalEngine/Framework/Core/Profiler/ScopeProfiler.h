#pragma once
#include "../Definitions.h"
#include "../Utility/Singleton.h"

namespace CE
{
	class CE_API ScopeProfiler : public Singleton
	{
	public:
		// Constructor with Scope name that is used as a BlockName
		ScopeProfiler(const char* ScopeName, bool IsPerformance);

		// Destructor
		~ScopeProfiler();

	private:
		const bool IsPerformance = false;
	};
}
