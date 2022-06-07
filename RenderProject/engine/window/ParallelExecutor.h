#pragma once
#include <chrono>
#include <vector>
#include <atomic>
#include <functional>
#include <thread>
#include <shared_mutex>?

#define BREAK __debugbreak();
#define ALWAYS_ASSERT(expression, ...)\
if (!(expression))\
{\
	BREAK;\
	std::abort();\
}

#ifdef NDEBUG
#define DEV_ASSERT(...)
#else
#define DEV_ASSERT(expression, ...) ALWAYS_ASSERT(expression, __VA_ARGS__);
#endif

struct ParallelExecutor
{
	bool m_isLooping;
	std::atomic<uint32_t> m_finishedThreadNum;
	std::atomic<uint32_t> m_completedBatchNum;
	std::function<void(uint32_t)> m_executeTasks;
	std::shared_mutex m_mutex;
	std::condition_variable_any m_waitCV;
	std::condition_variable_any m_workCV;
	std::vector<std::thread> m_threads;
	static const uint32_t MAX_THREADS; // 100% CPU occupation, it may cause OS hitches. No point to have more threads than the number of CPU logical cores.
	static const uint32_t HALF_THREADS; // 50-100% CPU occupation
	using Func = std::function<void(uint32_t, uint32_t)>; // (threadIndex, taskIndex)
	uint32_t numThreads() const { return m_threads.size(); }
	bool isWorking() const { return m_finishedThreadNum < m_threads.size(); }
	void wait()
	{
		if (!isWorking()) return;
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		if (!isWorking()) return; // re-check for a case when threads finished and m_waitCV is notified before the lock is acquired
		m_waitCV.wait(lock);
	}
	void execute(const Func& func, uint32_t numTasks, uint32_t tasksPerBatch)
	{
		if (numTasks == 0) return;
		executeAsync(func, numTasks, tasksPerBatch);
		wait();
	}
	void executeAsync(const Func& func, uint32_t numTasks, uint32_t tasksPerBatch)
	{
		if (numTasks == 0) return;
		DEV_ASSERT(tasksPerBatch > 0);
		wait(); // enforced waiting for completion of the previous parallel dispatch
		m_finishedThreadNum = 0;
		m_completedBatchNum = 0;
		uint32_t numBatches = (numTasks + tasksPerBatch - 1) / tasksPerBatch;
		m_executeTasks = [this, func, numTasks, numBatches, tasksPerBatch](uint32_t threadIndex)
		{
			while (true)
			{
				uint32_t batchIndex = m_completedBatchNum.fetch_add(1);
				if (batchIndex >= numBatches) return;
				uint32_t begin = (batchIndex + 0) * tasksPerBatch;
				uint32_t end = (batchIndex + 1) * tasksPerBatch;
				if (end > numTasks) end = numTasks;
				for (uint32_t taskIndex = begin; taskIndex < end; ++taskIndex)
					func(threadIndex, taskIndex);
			}
		};
		awake();
	}

	ParallelExecutor(uint32_t numThreads)
	{
		m_isLooping = true;
		DEV_ASSERT(numThreads > 0);
		m_finishedThreadNum = 0;
		m_threads.reserve(numThreads);
		for (uint32_t threadIndex = 0; threadIndex < numThreads; ++threadIndex)
			m_threads.emplace_back([this, threadIndex]() { loop(threadIndex); });
	}
	~ParallelExecutor()
	{
		wait();
		m_isLooping = false;
		awake();
		for (auto& t : m_threads) t.join();
	}

protected:
	void awake()
	{
		m_workCV.notify_all();
	}
	void loop(uint32_t threadIndex)
	{
		while (true)
		{
			{
				std::shared_lock<std::shared_mutex> lock(m_mutex);
				uint32_t prevFinishedNum = m_finishedThreadNum.fetch_add(1);
				if ((prevFinishedNum + 1) == m_threads.size())
				{
					m_executeTasks = {};
					m_waitCV.notify_all();
				}
				m_workCV.wait(lock);
			}
			if (!m_isLooping) return;
			DEV_ASSERT(m_executeTasks);
			m_executeTasks(threadIndex);
		}
	}
};