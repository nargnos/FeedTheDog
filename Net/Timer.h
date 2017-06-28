#ifndef TIMER_H_
#define TIMER_H_
#include "TlsPackage.h"
#include "TimerCallback.h"
namespace Detail
{

	template<typename TRep, typename TPeriod>
	void PostTimer(TimerPtr&& ptr, const std::chrono::duration<TRep, TPeriod>& duration)
	{
		assert(GlobalTlsPackage::Instance().TlsLoop != nullptr);
		GlobalTlsPackage::Instance().TlsTimerManager.Add(std::move(ptr), duration);
	}
	// void(const TimerCallback&)
	template<typename THandler, typename TRep, typename TPeriod>
	TimerPtr PostTimer(THandler&& cb, const std::chrono::duration<TRep, TPeriod>& duration)
	{
		auto tm = MakeTimerCallback(std::forward<THandler>(cb));
		auto ret = tm;
		PostTimer(std::move(tm), duration);
		return ret;
	}
}  // namespace Detail
using Detail::PostTimer;
#endif // TIMER_H_

