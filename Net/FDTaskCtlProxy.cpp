#include "FDTaskCtlProxy.h"
#include "Loop.h"
#include "EpollCpp.h"
void FDTaskCtlProxy::Add(Loop & loop, EpollOption op, IFDTask * obj)
{
	assert(obj != nullptr);
	loop.epoll_.Add(obj->FD(), op, obj);
}

void FDTaskCtlProxy::Mod(Loop & loop, EpollOption op, IFDTask * obj)
{
	assert(obj != nullptr);
	loop.epoll_.Mod(obj->FD(), op, obj);
}

void FDTaskCtlProxy::Del(Loop & loop, IFDTask * obj)
{
	assert(obj != nullptr);
	loop.epoll_.Del(obj->FD());
	// TODO: ×¢ÒâÉú´æÆÚ
}
