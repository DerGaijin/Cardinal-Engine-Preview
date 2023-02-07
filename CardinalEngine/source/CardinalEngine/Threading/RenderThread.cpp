#include "CardinalEngine/Threading/RenderThread.h"
#include "CardinalEngine/Threading/TaskSystem.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Framework/Platform/Platform.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"

namespace CE
{
	class RenderTask : public TaskBase
	{
	public:
		RenderTask(const FunctionRef<void(RHIContextImmediate&)>& Task) : m_Task(Task) {}

		virtual void Execute() override
		{
			if (m_Task != nullptr)
			{
				m_Task(Engine::Get().DynamicRHI().Immediate());
			}
		}

	private:
		const FunctionRef<void(RHIContextImmediate&)> m_Task;
	};

	SharedPtr<TaskBase> RenderThread::Enqueue(const FunctionRef<void(RHIContextImmediate&)>& Task)
	{
		SharedPtr<RenderTask> RT = SharedPtr<RenderTask>(new RenderTask(Task));
		Engine::Get().TaskSystem.Submit(EThreadType::Render, RT);
		return RT;
	}

	void RenderThread::RenderThreadMain()
	{
#if CE_DEBUG
		Platform::Get().SetCurrentThreadDebugName(L"Render Thread");
#endif

		Engine::Get().TaskSystem.ProcessQueueUntilExit();
	}
}
