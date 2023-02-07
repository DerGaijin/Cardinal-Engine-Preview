#pragma once
#include "../Framework/Core/Headers.h"
#include "TaskBase.h"

namespace CE
{
	template<typename Ret>
	class LambdaTask : public TaskBase
	{
	public:
		LambdaTask(const FunctionRef<Ret()>& Lambda) : m_Lambda(Lambda) {}

		virtual void Execute() override
		{
			Promise.set_value(m_Lambda());
		}

	public:
		std::promise<Ret> Promise;

	private:
		FunctionRef<Ret()> m_Lambda;
	};


	template<>
	class LambdaTask<void> : public TaskBase
	{
	public:
		LambdaTask(const FunctionRef<void()>& Lambda) : m_Lambda(Lambda) {}

		virtual void Execute() override
		{
			m_Lambda();
		}

	private:
		FunctionRef<void()> m_Lambda;
	};
}
