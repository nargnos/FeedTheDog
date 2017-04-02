#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_
#include <memory>
#include <queue>
#include <cassert>
#include "Task.h"
#include "Noncopyable.h"
class Loop;
// ���̰߳�ȫ
class TaskQueue :public Noncopyable
{
public:
	// ��֤˳��
	using TaskQueueType = std::queue<std::unique_ptr<ITask>>;
	TaskQueue();
	void Queue(std::unique_ptr<ITask>&& ptr);
	// ���û�п�ִ�е�����ͷ���true
	bool DoOnce(Loop& loop);
	void Clear();
	~TaskQueue() = default;
private:
	TaskQueueType queue_;
	TaskQueueType tmpQueue_;
};


#endif // TASKMANAGER_H_

