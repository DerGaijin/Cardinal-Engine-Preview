#pragma once
#include "../Framework/Core/Headers.h"
#include "TaskBase.h"
#include "LambdaTask.h"
#include "RenderThread.h"
#include "AudioThread.h"

namespace CE
{
	enum class EThreadType
	{
		Main = 0,
		Render,
		Audio,
		Worker,
		
		External,
	};

	class CE_API TaskSystem : public Singleton
	{
	private:
		struct TaskQueue
		{
			std::mutex Mutex;

			Array<SharedPtr<TaskBase>> Tasks;
			uint32 TasksProcessing = 0;

			std::condition_variable TasksAvailable;
			std::condition_variable QueueState;
		};

	public:
		bool Initialize();

		void Terminate();

	public:
		EThreadType ThreadType() const;
		
		bool Submit(EThreadType Queue, const SharedPtr<TaskBase>& Task);

		template<typename Ret>
		SharedPtr<LambdaTask<Ret>> Submit(EThreadType Queue, const FunctionRef<Ret()>& Lambda)
		{
			SharedPtr<LambdaTask<Ret>> Task = SharedPtr<LambdaTask<Ret>>(new LambdaTask<Ret>(Lambda));
			if (Submit(Queue, Task))
			{
				return Task;
			}
			return nullptr;
		}

		void ProcessQueueUntilIdle();
		
		void ProcessQueueUntilSignal(std::atomic<bool>& Signal);

		void ProcessQueueUntilExit();

		void WaitQueueIdle(EThreadType Queue);

		void WakeQueue(EThreadType Queue);

		void QueueStats(EThreadType Queue, size_t& RemainingTasks, uint32& TasksProcessing);

	private:
		bool HasAnyQueueTasks_Unsafe(const TaskQueue& Queue) const;

		bool IsQueueIdle_Unsafe(const TaskQueue& Queue) const;

		SharedPtr<TaskBase> GetTaskFromQueue_Unsafe(EThreadType Queue);

		void ExecuteTask(const SharedPtr<TaskBase>& Task);

		void WorkerMain(uint32 Index);

	public:
		RenderThread RenderThread;
		AudioThread AudioThread;

	private:
		std::atomic<bool> m_State = false;
		std::thread::id m_MainThreadID;

		Array<std::thread> m_WorkerThreads;

		TaskQueue m_TaskQueues[(int)EThreadType::External];
	};
}
