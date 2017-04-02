
#include "EpollCpp.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "EpollOption.h"
#include "Logger.h"
// 不接受任何可通过编程方式解决的errno
EpollCpp::EpollCpp() :
	epfd_(epoll_create1(EPOLL_CLOEXEC))

{
	if (epfd_ == -1)
	{
		assert(errno != EINVAL);
		TRACEPOINT_LINE(LogPriority::Debug)("Create Epoll Faild");
		TRACEERRNOEXIT(LogPriority::Emerg);
	}
}

EpollCpp::~EpollCpp()
{
	assert(epfd_ != -1);
	close(epfd_);
	epfd_ = -1;
}

void EpollCpp::Add(int fd, EpollOption op, const void * userdata) const
{
	EpollCtl(EPOLL_CTL_ADD, fd, CreateEpollEvent(op, userdata));
}

void EpollCpp::Mod(int fd, EpollOption op, const void * userdata) const
{
	EpollCtl(EPOLL_CTL_MOD, fd, CreateEpollEvent(op, userdata));
}

void EpollCpp::Del(int fd) const
{
	EpollCtl(EPOLL_CTL_DEL, fd, CreateEpollEvent(0, nullptr));
}

size_t EpollCpp::Wait(Events& e, int ms)
{
	assert(e.size() > 0);
	int nevent = 0;
	while (true)
	{
		nevent = epoll_wait(epfd_, e.data(), static_cast<int>(e.size()), ms);
		if (nevent == -1)
		{
			// 只允许EINTR，其它是编程错误
			if (errno == EINTR)
			{
				continue;
			}
			TRACEPOINT(LogPriority::Alert)("Loop Faild");
			TRACEERRNOEXIT(LogPriority::Alert);
		}
		break;
	}
	return static_cast<size_t>(nevent);
}

epoll_event EpollCpp::CreateEpollEvent(EpollOption op, const void * userdata)
{
	epoll_event ev;
	ev.events = op.Value;
	ev.data.ptr = const_cast<void*>(userdata);
	return ev;
}

void EpollCpp::EpollCtl(int flag, int fd, const epoll_event & ev) const
{
	assert(epfd_ != -1);
	assert(fd != -1);
	if (epoll_ctl(epfd_, flag, fd, const_cast<epoll_event*>(&ev)) == -1)
	{
		assert(errno == ENOMEM || errno == ENOSPC);
		TRACEPOINT(LogPriority::Debug).Errno();
		TRACEERRNOEXIT(LogPriority::Emerg);
	}
}
