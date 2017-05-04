#include "FDTaskCtlAttorney.h"
#include "Loop.h"
#include "EpollCpp.h"
void FDTaskCtlAttorney::Add(Loop & loop, EpollOption op, IFDTask * obj)
{
	assert(obj != nullptr);
	loop.epoll_.Add(obj->FD(), op, obj);
}

void FDTaskCtlAttorney::Mod(Loop & loop, EpollOption op, IFDTask * obj)
{
	assert(obj != nullptr);
	loop.epoll_.Mod(obj->FD(), op, obj);
}

void FDTaskCtlAttorney::Del(Loop & loop, IFDTask * obj)
{
	assert(obj != nullptr);
	loop.epoll_.Del(obj->FD());
	// TODO: ×¢ÒâÉú´æÆÚ
}
