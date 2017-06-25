
#include "EpollCpp.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "EpollOption.h"
#include "Logger.h"
namespace Detail
{
	// 不接受任何可通过编程方式解决的errno
	EpollCpp::EpollCpp() :
		epfd_(epoll_create1(EPOLL_CLOEXEC))
	{
		if (epfd_ == -1)
		{
			assert(errno != EINVAL);
			TRACEPOINT_LINE(LogPriority::Debug)("Create Epoll Failed");
			TRACEERRNOEXIT(LogPriority::Emerg);
		}
	}

	EpollCpp::~EpollCpp()
	{
		if (epfd_ != -1)
		{
			close(epfd_);
			epfd_ = -1;
		}
	}

	void EpollCpp::Add(int fd, EpollOption op, const void * userdata) const
	{
		EpollCtl(EPOLL_CTL_ADD, fd, epoll_event{ op, const_cast<void*>(userdata) });
	}

	void EpollCpp::Mod(int fd, EpollOption op, const void * userdata) const
	{
		EpollCtl(EPOLL_CTL_MOD, fd, epoll_event{ op, const_cast<void*>(userdata) });
	}

	void EpollCpp::Del(int fd) const
	{
		EpollCtl(EPOLL_CTL_DEL, fd, epoll_event{ 0,nullptr });
	}

	size_t EpollCpp::Wait(Events& e, int ms)
	{
		assert(e.size() > 0);
		assert(epfd_ != -1);
		int nevent = 0;
		while (true)
		{
			nevent = epoll_wait(epfd_, e.data(), static_cast<int>(e.size()), ms);
			if (__glibc_unlikely(nevent == -1))
			{
				// 只允许EINTR，其它是编程错误
				if (errno == EINTR)
				{
					continue;
				}
				TRACEPOINT(LogPriority::Alert)("Loop Failed");
				TRACEERRNOEXIT(LogPriority::Alert);
			}
			break;
		}
		assert(nevent >= 0);
		return static_cast<size_t>(nevent);
	}

	void EpollCpp::EpollCtl(int flag, int fd, const epoll_event & ev) const
	{
		assert(epfd_ != -1);
		assert(fd != -1);
		if (__glibc_unlikely(epoll_ctl(epfd_, flag, fd, const_cast<epoll_event*>(&ev)) == -1))
		{
			assert(errno == ENOMEM || errno == ENOSPC);
			TRACEPOINT(LogPriority::Debug).Errno();
			TRACEERRNOEXIT(LogPriority::Emerg);
		}
	}

}  // namespace Detail