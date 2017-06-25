#ifndef PROACTORCONNECTION_H_
#define PROACTORCONNECTION_H_
#include <cassert>
#include <memory>
#include <queue>
#include <atomic>
#include <algorithm>
#include <sys/epoll.h>
#include "IConnection.h"
#include "Noncopyable.h"
#include "FDTaskCtlAttorney.h"
#include "Operator.h"
#include "Loop.h"
#include "RegisterTaskAttorney.h"
#include "TidChecker.h"
namespace Detail
{
	class ConnectionAttorney;
	class Socket;
	class ProactorConnection :
		public IConnection,
		public std::enable_shared_from_this<IConnection>,
		public Noncopyable,
		public TidChecker
	{
	protected:
		using OpPtr = std::unique_ptr<Operator>;
		using OpQueue = std::queue<OpPtr>;
	public:
		friend class ConnectionAttorney;
		ProactorConnection(Loop& loop, Socket* socketPtr);
		virtual ~ProactorConnection();

		void Close();
	protected:
		void ClearQueues(Error err);
		void RegTask();

		void CheckAndRegTask(EpollOption e);
		void PostReadOp(OpPtr&& op);
		void PostWriteOp(OpPtr&& op);
		bool HasError() const;
		EpollOption CurrentEvent()const;
		bool IsAllOpQueueEmpty();
		void Protect();
		void Destory();
		void RegSocket();
		void UnRegSocket();
		void SetNonBlocking();
		void Recycle();
		void CheckFd();
		virtual int FD() const override;
	private:
		virtual void DoEvent(Loop & loop, EpollOption op) override;
		virtual bool DoEvent(Loop & loop) override;

		static void ClearQueue(OpQueue& q, Error err);

		enum
		{
			ReadOp = 0,
			WriteOp = 1,
			MaxOps = 2
		};
		OpQueue opqueue_[MaxOps];
		typename std::list<std::shared_ptr<IConnection>>::const_iterator storePos_;
		Loop& loop_;
		Socket* socketPtr_;
		EpollOption currentEvent_;
		bool isSocketRegistered_;
		bool isTaskRegistered_;
		// 是否已保护不被析构
		bool isProtected_;
	};



}  // namespace Detail
#endif // PROACTORCONNECTION_H_

