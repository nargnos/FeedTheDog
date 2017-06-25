#ifndef TIMERFD_H_
#define TIMERFD_H_
#include <sys/timerfd.h>
#include <chrono>
#include <cassert>
#include "Noncopyable.h"
namespace Detail
{
	enum class TimerFdTypes :clockid_t
	{
		Realtime = CLOCK_REALTIME,
		Monotonic = CLOCK_MONOTONIC,
		// ProcessCputimeId = CLOCK_PROCESS_CPUTIME_ID,
		// ThreadCputimeId = CLOCK_THREAD_CPUTIME_ID,
		// MonotonicRaw = CLOCK_MONOTONIC_RAW,
		// RealtimeCoarse = CLOCK_REALTIME_COARSE,
		// MonotonicCoarse = CLOCK_MONOTONIC_COARSE,
		Boottime = CLOCK_BOOTTIME,
		RealtimeAlarm = CLOCK_REALTIME_ALARM,
		BoottimeAlarm = CLOCK_BOOTTIME_ALARM,
		// Tai = CLOCK_TAI,
	};
	enum class TimerFdFlags :int
	{
		None=0,
		Cloexec = TFD_CLOEXEC,
		Nonblock = TFD_NONBLOCK,
	};
	enum class TimerSetModes :int
	{
		// 相对时间
		None = 0,
		// 绝对时间
		AbsTime = TFD_TIMER_ABSTIME
	};
	class TimerFd :
		public Noncopyable
	{
	public:

		TimerFd(TimerFdTypes clockid, TimerFdFlags flag);
		~TimerFd();
		int FD()const;
		void SetTime(TimerSetModes mode, const itimerspec* newVal, itimerspec* oldVal);
		void GetTIme(itimerspec* out);
		void Close();
	private:
		int fd_;
	};
	template<typename TDuration>
	timespec MakeTimerSpec(const TDuration& val)
	{
		using namespace std::chrono;
		assert(val >= std::chrono::nanoseconds::zero());
		return timespec{
			duration_cast<seconds>(val).count(),
			(duration_cast<nanoseconds>(val) - duration_cast<seconds>(val)).count()
		};
	}
	template<typename TInterval, typename TDuration>
	itimerspec MakeITimerSpec(const TInterval& interval, const TDuration& duration)
	{		
		return itimerspec{ MakeTimerSpec(interval) ,MakeTimerSpec(duration) };
	}

}  // namespace Detail
#endif // TIMERFD_H_

