#include "EventFd.h"
#include <cassert>
#include <unistd.h>
#include <stdlib.h>
#include "Logger.h"
EventFd::EventFd(unsigned int count, EventFdFlags flags) :fd_(eventfd(count, flags))
{
	if (fd_ == -1)
	{
		TRACEPOINT_LINE(LogPriority::Debug);
		TRACEERRNOEXIT(LogPriority::Emerg);
	}
}


EventFd::~EventFd()
{
	assert(fd_ != -1);
	close(fd_);
}

int EventFd::FD() const
{
	return fd_;
}

bool EventFd::Read(eventfd_t * val) const
{
	assert(fd_ != -1);
	assert(val != nullptr);
	return eventfd_read(fd_, val) != -1;
}

bool EventFd::Write(eventfd_t val) const
{
	assert(fd_ != -1);
	return eventfd_write(fd_, val) != -1;
}
