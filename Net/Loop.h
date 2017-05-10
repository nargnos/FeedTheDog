
#ifndef LOOP_H_
#define LOOP_H_
#include <memory>
#include <thread>
#include <type_traits>
#include <list>
#include <functional>
#include "Noncopyable.h"
#include "EpollOption.h"
#include "IFDTask.h"
#include "ITask.h"
#include "TaskList.h"
#include "EpollCpp.h"
#include "EventTaskBase.h"
namespace Detail
{
	class FDTaskCtlAttorney;
	class RunAttorney;
	class RegisterTaskAttorney;
	// 只对自身注册的任务暴露自身非线程安全的接口
	class Loop final :
		public EventTaskBase
	{
	public:
		enum LoopState:char
		{
			Running,
			Stopping,
			Stopped
		};
		friend FDTaskCtlAttorney;
		friend RunAttorney;
		friend RegisterTaskAttorney;
		Loop();
		~Loop();
		LoopState State() const;
		std::thread::id OwnerTid()const;
		// 取任务数快照
		int TaskCount() const;
	private:
		virtual void DoEvent(Loop & loop, EpollOption op) override;

		// 非线程安全
		void RegisterTask(std::shared_ptr<ITask>&& ptr);
		// 不可重复执行
		void Start();
		void DoLoop();
		void PrepareBuffers();
		// 会导致未完成任务被丢弃
		void Stop();
		bool CheckTid() const;
		EpollCpp& Epoll();

		TaskList taskList_;
		EpollCpp epoll_;
		std::thread::id tid_;
		int loopID_;
		LoopState state_;
		static const int Infinite = -1;
	};
}  // namespace Detail

#endif /* LOOP_H_ */
