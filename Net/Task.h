#ifndef TASK_H_
#define TASK_H_
#include <utility>
#include <memory>
#include "ITask.h"
#include "Noncopyable.h"
using ITaskPtr = std::unique_ptr<ITask>;
template<typename TFunc>
class Task :
	public Noncopyable,
	public ITask
{
public:

	explicit Task(TFunc&& func) :func_(std::move(func))
	{
	}
	virtual void DoEvent(Loop& loop, ITaskPtr&& self)  override
	{
		func_(loop, std::move(self));
	}
	~Task() = default;
private:
	TFunc func_;
};
// »Øµ÷ÉùÃ÷ void(Loop& loop, ITaskPtr&& self)
template<typename TFunc>
ITaskPtr MakeTask(TFunc&& func)
{
	return ITaskPtr(new Task<TFunc>(std::move(func)));
}

#endif // TASK_H_

