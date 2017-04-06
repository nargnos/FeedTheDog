#include "TaskList.h"

void TaskList::Register(std::unique_ptr<ITask>&& ptr)
{
	taskList_.push_back(std::move(ptr));
}

bool TaskList::DoOnce(Loop & loop)
{
	if (taskList_.empty())
	{
		return true;
	}
	auto it = taskList_.begin();
	auto end = taskList_.end();
	while (it != end)
	{
		auto& task = *it;
		if (task->DoEvent(loop, std::move(task)))
		{
			it = taskList_.erase(it);
		}
		else
		{
			++it;
		}
	}
	return taskList_.empty();
}

void TaskList::Clear()
{
	taskList_.clear();
}
