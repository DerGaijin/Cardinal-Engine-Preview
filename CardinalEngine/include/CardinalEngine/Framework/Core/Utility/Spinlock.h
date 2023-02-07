#pragma once
#include "../Definitions.h"

#include <atomic>

namespace CE
{
	class CE_API Spinlock
	{
	public:
		void lock();

		void unlock();

	private:
		std::atomic_flag m_Flag = ATOMIC_FLAG_INIT;
	};
}
