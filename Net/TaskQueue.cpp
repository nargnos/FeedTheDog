#include "TaskQueue.h"

TaskQueue::TaskQueue()
{
}

bool TaskQueue::DoOnce(Loop& loop)
{
	if (queue_.empty())
	{
		return true;
	}
	// 交换队列，防止重新插入造成循环
	queue_.swap(tmpQueue_);
	assert(!tmpQueue_.empty());
	do
	{
		auto& task = tmpQueue_.front();
		task->DoEvent(loop, std::move(task));
		tmpQueue_.pop();
	} while (!tmpQueue_.empty());

	return queue_.empty();
}

void TaskQueue::Queue(std::unique_ptr<ITask>&& ptr)
{
	assert(ptr);
	queue_.push(std::move(ptr));
}

void TaskQueue::Clear()
{
	TaskQueueType().swap(queue_);
	TaskQueueType().swap(tmpQueue_);
}

