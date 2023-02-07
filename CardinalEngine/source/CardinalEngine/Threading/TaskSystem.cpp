#include "CardinalEngine/Threading/TaskSystem.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Framework/Platform/Platform.h"

namespace CE
{
	bool TaskSystem::Initialize()
	{
		m_MainThreadID = std::this_thread::get_id();

		uint32 WorkerCount = std::thread::hardware_concurrency();

		// Override Worker Count By Config
		if (Engine::Get().Config.HasKey("Multithreading", "WorkerCount"))
		{
			uint32 OverriddenWorkerCount = WorkerCount;
			if (Engine::Get().Config["Multithreading"]["WorkerCount"].ToUnsignedIntegral(OverriddenWorkerCount))
			{
				WorkerCount = OverriddenWorkerCount;
			}
		}

		m_State.store(true);

		m_WorkerThreads.Clear();
		m_WorkerThreads.AddDefaulted(WorkerCount);
		uint32 WT_Index = 0;
		for (auto& WT : m_WorkerThreads)
		{
			WT = std::thread(std::bind(&TaskSystem::WorkerMain, this, WT_Index));
			WT_Index++;
		}

		RenderThread.m_Thread = std::thread(&RenderThread::RenderThreadMain, &RenderThread);

		return true;
	}

	void TaskSystem::Terminate()
	{
		m_State.store(false);

		for (uint32 i = 0; i < (int)EThreadType::External; i++)
		{
			m_TaskQueues[i].TasksAvailable.notify_all();
			m_TaskQueues[i].QueueState.notify_all();
		}

		if (RenderThread.m_Thread.joinable())
		{
			RenderThread.m_Thread.join();
		}

		for (auto& WT : m_WorkerThreads)
		{
			if (WT.joinable())
			{
				WT.join();
			}
		}
		m_WorkerThreads.Clear();

		for (uint32 i = 0; i < (int)EThreadType::External; i++)
		{
			m_TaskQueues[i].Tasks.Clear();
		}

		m_MainThreadID = std::thread::id();
	}

	EThreadType TaskSystem::ThreadType() const
	{
		const std::thread::id CurrentID = std::this_thread::get_id();
		if (CurrentID == std::thread::id())
		{
			return EThreadType::External;
		}

		if (CurrentID == m_MainThreadID)
		{
			return EThreadType::Main;
		}

		if (CurrentID == RenderThread.m_Thread.get_id())
		{
			return EThreadType::Render;
		}

		if (CurrentID == AudioThread.m_Thread.get_id())
		{
			return EThreadType::Audio;
		}

		for (auto& WT : m_WorkerThreads)
		{
			if (CurrentID == WT.get_id())
			{
				return EThreadType::Worker;
			}
		}

		return EThreadType::External;
	}

	bool TaskSystem::Submit(EThreadType Queue, const SharedPtr<TaskBase>& Task)
	{
		if (Task == nullptr)
		{
			return false;
		}

		ETaskState Excpected = ETaskState::Idle;
		if (!Task->m_State.compare_exchange_weak(Excpected, ETaskState::Enqueued))
		{
			CE_LOG(ELog::Warning, "Tried to enqueue Task that is not in 'Idle' State");
			return false;
		}

		Task->m_Promise = std::promise<void>();
		Task->m_Future = Task->m_Promise.get_future();

		if (Queue >= EThreadType::External)
		{
			// Create a New Thread
			CE_CHECK(false);
		}
		else
		{
			TaskQueue& TaskQueue = m_TaskQueues[(int)Queue];
			{
				std::unique_lock<std::mutex> Lock(TaskQueue.Mutex);
				TaskQueue.Tasks.Emplace(Task);
				TaskQueue.TasksAvailable.notify_all();
			}
		}

		return true;
	}

	void TaskSystem::ProcessQueueUntilIdle()
	{
		EThreadType MyType = ThreadType();
		if (MyType == EThreadType::External)
		{
			return;
		}

		TaskQueue& Queue = m_TaskQueues[(int)MyType];
		while (m_State)
		{
			SharedPtr<TaskBase> Task;

			{
				std::unique_lock<std::mutex> Lock(Queue.Mutex);
				Task = GetTaskFromQueue_Unsafe(MyType);

				if (Task != nullptr)
				{
					Queue.TasksProcessing++;
				}
			}

			if (Task == nullptr)
			{
				break;
			}

			ExecuteTask(Task);

			if (Task != nullptr)
			{
				std::unique_lock<std::mutex> Lock(Queue.Mutex);
				Queue.TasksProcessing--;
				if (Queue.TasksProcessing == 0)
				{
					Queue.QueueState.notify_all();
				}
			}
		}
	}

	void TaskSystem::ProcessQueueUntilSignal(std::atomic<bool>& Signal)
	{
		EThreadType MyType = ThreadType();
		if (MyType == EThreadType::External)
		{
			return;
		}

		TaskQueue& TQueue = m_TaskQueues[(int)MyType];
		while (m_State && Signal)
		{
			SharedPtr<TaskBase> Task;

			{
				std::unique_lock<std::mutex> Lock(TQueue.Mutex);

				Task = GetTaskFromQueue_Unsafe(MyType);

				if (Task == nullptr)
				{
					TQueue.TasksAvailable.wait(Lock, [&]() { return !m_State.load() || !Signal.load() || HasAnyQueueTasks_Unsafe(TQueue); });
				}

				if (Task == nullptr)
				{
					Task = GetTaskFromQueue_Unsafe(MyType);
				}

				if (Task != nullptr)
				{
					TQueue.TasksProcessing++;
				}
			}

			ExecuteTask(Task);

			if (Task != nullptr)
			{
				std::unique_lock<std::mutex> Lock(TQueue.Mutex);
				TQueue.TasksProcessing--;
				if (TQueue.TasksProcessing == 0)
				{
					TQueue.QueueState.notify_all();
				}
			}
		}
	}

	void TaskSystem::ProcessQueueUntilExit()
	{
		ProcessQueueUntilSignal(m_State);
	}

	void TaskSystem::WaitQueueIdle(EThreadType Queue)
	{
		if (Queue >= EThreadType::External)
		{
			return;
		}

		TaskQueue& TQueue = m_TaskQueues[(int)Queue];

		while (m_State)
		{
			std::unique_lock<std::mutex> Lock(TQueue.Mutex);

			if (IsQueueIdle_Unsafe(TQueue))
			{
				break;
			}

			TQueue.QueueState.wait(Lock, [&]() { return !m_State.load() || IsQueueIdle_Unsafe(TQueue); });

			if (IsQueueIdle_Unsafe(TQueue))
			{
				break;
			}
		}
	}

	void TaskSystem::WakeQueue(EThreadType Queue)
	{
		if (Queue >= EThreadType::External)
		{
			return;
		}

		TaskQueue& TQueue = m_TaskQueues[(int)Queue];

		TQueue.TasksAvailable.notify_all();
		TQueue.QueueState.notify_all();
	}

	void TaskSystem::QueueStats(EThreadType Queue, size_t& RemainingTasks, uint32& TasksProcessing)
	{
		if (Queue >= EThreadType::External)
		{
			return;
		}

		TaskQueue& TQueue = m_TaskQueues[(int)Queue];
		std::lock_guard<std::mutex> Lock(TQueue.Mutex);
		RemainingTasks = TQueue.Tasks.Size();
		TasksProcessing = TQueue.TasksProcessing;
	}

	bool TaskSystem::HasAnyQueueTasks_Unsafe(const TaskQueue& Queue) const
	{
		return Queue.Tasks.Size() > 0;
	}

	bool TaskSystem::IsQueueIdle_Unsafe(const TaskQueue& Queue) const
	{
		return Queue.Tasks.Size() == 0 && Queue.TasksProcessing == 0;
	}

	SharedPtr<TaskBase> TaskSystem::GetTaskFromQueue_Unsafe(EThreadType Queue)
	{
		if (Queue >= EThreadType::External)
		{
			return nullptr;
		}

		TaskQueue& TaskQueue = m_TaskQueues[(int)Queue];
		if (TaskQueue.Tasks.Size() > 0)
		{
			SharedPtr<TaskBase> Result = TaskQueue.Tasks[0];
			TaskQueue.Tasks.RemoveAt(0);
			return Result;
		}

		return nullptr;
	}

	void TaskSystem::ExecuteTask(const SharedPtr<TaskBase>& Task)
	{
		if (Task != nullptr)
		{
			ETaskState Expected = ETaskState::Enqueued;
			if (Task->m_State.compare_exchange_weak(Expected, ETaskState::Executing))
			{
				Task->Execute();
				Task->m_Promise.set_value();

				Expected = ETaskState::Executing;
				Task->m_State.compare_exchange_weak(Expected, ETaskState::Completed);
			}
			else
			{
				CE_LOG(ELog::Warning, "Tried to execute a Task that is not in 'Enqueued' State");
			}
		}
	}

	void TaskSystem::WorkerMain(uint32 Index)
	{
#if CE_DEBUG
		Platform::Get().SetCurrentThreadDebugName(Text::Format(L"Worker %u", Index));
#endif

		ProcessQueueUntilExit();
	}
}
