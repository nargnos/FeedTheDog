#include "TimerManager.h"

Detail::TimerManager::TimerManager() :
	fd_(TimerFdTypes::Realtime, TimerFdFlags::Cloexec)
{
}

void Detail::TimerManager::DoEvent(Loop & loop, EpollOption op)
{
	if (op&EPOLLERR)
	{
		assert(false);
		return;
	}
	auto next = th_.Do();
	SetTime(next);
}
void Detail::TimerManager::SetTime(long long ms)
{
	auto t = MakeITimerSpec(std::chrono::seconds::zero(), std::chrono::milliseconds(ms));
	fd_.SetTime(TimerSetModes::None, &t, nullptr);
}
int Detail::TimerManager::FD() const
{
	return fd_.FD();
}
