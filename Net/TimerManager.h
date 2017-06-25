#ifndef TIMERMANAGER_H_
#define TIMERMANAGER_H_
#include "TimeHeap.h"
#include "TimerFd.h"
#include "IFDTask.h"
#include "TidChecker.h"
namespace Detail
{
	// 非线程安全，要放tls
	class TimerManager :
		public IFDTask,
		public TidChecker
	{
	public:

		TimerManager();

		template<typename T>
		void Add(TimerPtr&& ptr, T&& duration)
		{
			CheckTid();
			auto ms = th_.Add(std::move(ptr), std::forward<T>(duration));
			SetTime(ms);
		}

	private:
		TimeHeap th_;
		TimerFd fd_;

		virtual void DoEvent(Loop & loop, EpollOption op) override;
		void SetTime(long long ms);
		virtual int FD() const override;
	};

}  // namespace Detail
#endif // TIMERMANAGER_H_

