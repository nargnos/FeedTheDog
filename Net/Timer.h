#ifndef TIMER_H_
#define TIMER_H_
#include "TlsPackage.h"
#include "TimerCallback.h"
namespace Detail
{

	template<typename T>
	void PostTimer(TimerPtr&& ptr, T&& duration)
	{
		GlobalTlsPackage::Instance().TlsTimerManager.Add(std::move(ptr), std::forward<T>(duration));
	}
	// void(const TimerCallback&)
	template<typename THandler, typename T>
	TimerPtr PostTimer(THandler&& cb, T&& duration)
	{
		auto tm = MakeTimerCallback(std::forward<THandler>(cb));
		auto ret = tm;
		PostTimer(std::move(tm), std::forward<T>(duration));
		return ret;
	}
}  // namespace Detail
using Detail::PostTimer;
#endif // TIMER_H_

