#pragma once
#include "../Framework/Core/Headers.h"
#include "TaskBase.h"

namespace CE
{
	class RHIContextImmediate;

	class CE_API RenderThread : public NonCopyable
	{
		friend class TaskSystem;
	public:
		static SharedPtr<TaskBase> Enqueue(const FunctionRef<void(RHIContextImmediate&)>& Task);
	
	private:
		void RenderThreadMain();

	private:
		std::thread m_Thread;
	};
}
