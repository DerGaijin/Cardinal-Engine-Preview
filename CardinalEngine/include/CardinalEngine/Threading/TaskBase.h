#pragma once
#include "../Framework/Core/Headers.h"

namespace CE
{
	enum class ETaskState
	{
		Idle,
		Enqueued,
		Executing,
		Completed,
	};


	class CE_API TaskBase
	{
		friend class TaskSystem;

	public:
		virtual ~TaskBase() = default;

		virtual void Execute() = 0;

		bool Reset();

	public:
		ETaskState State() const
		{
			return m_State;
		}

		void Wait()
		{
			m_Future.wait();
		}

		template<typename Rep, typename Period>
		std::future_status WaitFor(const std::chrono::duration<Rep, Period>& Duration)
		{
			return m_Future.wait_for(Duration);
		}

		template<typename Clock, typename Dur>
		std::future_status WaitUntil(const std::chrono::time_point<Clock, Dur>& TimePoint)
		{
			return m_Future.wait_until(TimePoint);
		}

	private:
		std::atomic<ETaskState> m_State = ETaskState::Idle;
		std::promise<void> m_Promise;
		std::future<void> m_Future;
	};
}
