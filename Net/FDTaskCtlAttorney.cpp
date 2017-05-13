#include "FDTaskCtlAttorney.h"
#include "Loop.h"
#include "EpollCpp.h"
namespace Detail
{
	void FDTaskCtlAttorney::Add(const Loop & loop, EpollOption op, const IFDTask * obj)
	{
		assert(obj != nullptr);
		loop.epoll_.Add(obj->FD(), op, obj);
	}

	void FDTaskCtlAttorney::Mod(const Loop & loop, EpollOption op, const IFDTask * obj)
	{
		assert(obj != nullptr);
		loop.epoll_.Mod(obj->FD(), op, obj);
	}

	void FDTaskCtlAttorney::Del(const Loop & loop, const IFDTask * obj)
	{
		assert(obj != nullptr);
		loop.epoll_.Del(obj->FD());
		// TODO: 注意生存期
	}

}  // namespace Detail