#ifndef TASK_H_
#define TASK_H_
#include <memory>
#include "ITask.h"
#include "Noncopyable.h"

namespace Detail
{
	using ITaskPtr = std::shared_ptr<ITask>;

	template<typename TFunc>
	class TaskBase :
		public Noncopyable
	{
	public:
		explicit TaskBase(TFunc&& func) :
			func_(std::forward<TFunc>(func))
		{
		}
		virtual ~TaskBase() = default;
	protected:
		void Invoke(Loop& loop) const
		{
			func_(loop);
		}
	private:
		TFunc func_;
	};

	template<typename TFunc>
	class Task :
		public TaskBase<TFunc>,
		public ITask
	{
	public:
		using TaskBase<TFunc>::TaskBase;
	private:
		virtual bool DoEvent(Loop& loop)  override
		{
			return this->Invoke(loop);
		}
	};
	// 回调声明 void(Loop& loop)
	template<typename TFunc>
	ITaskPtr MakeTask(TFunc&& func)
	{
		return ITaskPtr(new Task<TFunc>(std::forward<TFunc>(func)));
	}
	// 线程不安全
	void PostTask(Loop& loop, ITaskPtr&& task);
	template<typename TFunc>
	void PostTask(Loop& loop, TFunc&& func)
	{
		PostTask(loop, MakeTask(func));
	}

}  // namespace Detail
using Detail::MakeTask;
using Detail::PostTask;
#endif // TASK_H_

