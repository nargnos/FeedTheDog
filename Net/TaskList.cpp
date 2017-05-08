#include "TaskList.h"

void TaskList::Register(std::shared_ptr<ITask>&& ptr)
{
	taskList_.emplace_back(std::move(ptr));
}

bool TaskList::DoOnce(Loop & loop)
{
	auto it = taskList_.begin();
	auto end = taskList_.end();
	while (it != end)
	{
		if (__glibc_unlikely((*it)->DoEvent(loop)))
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

size_t TaskList::Count() const
{
	return taskList_.size();
}
