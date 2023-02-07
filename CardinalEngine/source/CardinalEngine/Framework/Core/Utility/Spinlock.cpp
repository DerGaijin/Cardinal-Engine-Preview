#include "CardinalEngine/Framework/Core/Utility/Spinlock.h"

namespace CE
{
	void Spinlock::lock()
	{
		while (m_Flag.test_and_set(std::memory_order_acquire));
	}

	void Spinlock::unlock()
	{
		m_Flag.clear(std::memory_order_release);
	}
}
