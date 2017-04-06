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
	// ����true��task����б����Ƴ�
	void Register(std::unique_ptr<ITask>&& ptr);
	// ���û�п�ִ�е�����ͷ���true
	bool DoOnce(Loop& loop);
	void Clear();
private:
	std::list<std::unique_ptr<ITask>> taskList_;
};


#endif // TASKLIST_H_

