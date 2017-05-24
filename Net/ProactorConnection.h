#ifndef PROACTORCONNECTION_H_
#define PROACTORCONNECTION_H_
#include <cassert>
#include <memory>
#include <queue>
#include <atomic>
#include <algorithm>
#include <sys/epoll.h>
#include "ITask.h"
#include "IFDTask.h"
#include "Noncopyable.h"
#include "FDTaskCtlAttorney.h"
#include "Operator.h"
#include "Loop.h"
#include "RegisterTaskAttorney.h"
namespace Detail
{
	class ProactorConnection :
		public ITask,
		public IFDTask,
		public std::enable_shared_from_this<ITask>,
		public Noncopyable
	{
	protected:
		using OpPtr = std::unique_ptr<Operator>;
		using OpQueue = std::queue<OpPtr>;
	public:
		explicit ProactorConnection(Loop& loop);

		virtual ~ProactorConnection();
	protected:
		void RegTask();

		void CheckAndRegTask(EpollOption e);
		void PostReadOp(OpPtr&& op);
		void PostWriteOp(OpPtr&& op);
		// FIX: 有隐患（这样做会无法顺畅终止程序），
		// 用boost的ObjectPool管理就不需要用这个，也可以取消掉智能指针的使用
		void Protect();
		void Destory();
		bool HasError() const;
		EpollOption CurrentEvent()const;
	private:
		void RegSocket();
		void UnRegSocket();
		virtual void DoEvent(Loop & loop, EpollOption op) override;
		virtual bool DoEvent(Loop & loop) override;

		void ClearQueue(OpQueue& q, Error err);

		bool IsAllOpQueueEmpty();


		enum
		{
			ReadOp = 0,
			WriteOp = 1,
			MaxOps = 2
		};
		OpQueue opqueue_[MaxOps];
		std::shared_ptr<ITask> store_;
		Loop& loop_;
		EpollOption currentEvent_;
		bool isSocketRegistered_;
		bool isTaskRegistered_;
	};



}  // namespace Detail
#endif // PROACTORCONNECTION_H_

