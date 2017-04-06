#ifndef TASKLIST_H_
#define TASKLIST_H_
#include <list>
#include <memory>
#include "ITask.h"
#include "Noncopyable.h"
class Loop;
class TaskList :public Noncopyable
{
public:

	TaskList() = default;

	~TaskList() = default;
	// 返回true的task会从列表中移除
	void Register(std::unique_ptr<ITask>&& ptr);
	// 如果没有可执行的任务就返回true
	bool DoOnce(Loop& loop);
	void Clear();
private:
	std::list<std::unique_ptr<ITask>> taskList_;
};


#endif // TASKLIST_H_

