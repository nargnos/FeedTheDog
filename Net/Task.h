#ifndef TASK_H_
#define TASK_H_
#include <memory>
#include "ITask.h"
#include "Noncopyable.h"
namespace Detail
{
	using ITaskPtr = std::shared_ptr<ITask>;
	template<typename TFunc>
	class Task :
		public Noncopyable,
		public ITask
	{
	public:

		explicit Task(TFunc&& func) :
			func_(std::forward<TFunc>(func))
		{
		}
		virtual bool DoEvent(Loop& loop)  override
		{
			return func_(loop);
		}
	private:
		TFunc func_;
	};
	// 回调声明 void(Loop& loop)
	template<typename TFunc>
	ITaskPtr MakeTask(TFunc&& func)
	{
		return ITaskPtr(new Task<TFunc>(std::forward<TFunc>(func)));
	}

}  // namespace Detail
#endif // TASK_H_

