#include "CardinalEngine/Threading/TaskBase.h"

namespace CE
{
	bool TaskBase::Reset()
	{
		ETaskState Expected = ETaskState::Completed;
		if (!m_State.compare_exchange_weak(Expected, ETaskState::Idle))
		{
			if (Expected != ETaskState::Idle)
			{
				CE_LOG(ELog::Warning, "Tried to Reset a Task that is not in 'Completed' State");
			}
			return false;
		}

		return true;
	}
}
