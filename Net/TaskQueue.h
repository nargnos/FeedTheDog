#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_
#include <memory>
#include <queue>
#include <cassert>
#include "Task.h"
#include "Noncopyable.h"
class Loop;
// 非线程安全
class TaskQueue :public Noncopyable
{
public:
	// 保证顺序
	using TaskQueueType = std::queue<std::unique_ptr<ITask>>;
	TaskQueue();
	void Queue(std::unique_ptr<ITask>&& ptr);
	// 如果没有可执行的任务就返回true
	bool DoOnce(Loop& loop);
	void Clear();
	~TaskQueue() = default;
private:
	TaskQueueType queue_;
	TaskQueueType tmpQueue_;
};


#endif // TASKMANAGER_H_

