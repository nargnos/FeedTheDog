#include "TimerFd.h"
#include "Logger.h"
Detail::TimerFd::TimerFd(TimerFdTypes clockid, TimerFdFlags flag) :
	fd_(timerfd_create(static_cast<clockid_t>(clockid), static_cast<int>(flag)))
{
	if (fd_ == -1)
	{
		TRACEPOINT_LINE(LogPriority::Debug);
		TRACEERRNOEXIT(LogPriority::Emerg);
	}
}

Detail::TimerFd::~TimerFd()
{
	if (fd_ == -1)
	{
		return;
	}
	close(fd_);
	fd_ = -1;
}

int Detail::TimerFd::FD() const
{
	return fd_;
}

void Detail::TimerFd::SetTime(TimerSetModes mode, const itimerspec * newVal, itimerspec * oldVal)
{
	auto ret = timerfd_settime(fd_, static_cast<int>(mode), newVal, oldVal);
	// ����ֻ����Ϊ��������(flag�����newValԽ��)���������κδ���
	assert(ret != -1);
}

void Detail::TimerFd::GetTIme(itimerspec * out)
{
	auto ret = timerfd_gettime(fd_, out);
	assert(ret != -1);
}

void Detail::TimerFd::Close()
{
	if (fd_ != -1)
	{
		close(fd_);
		fd_ = -1;
	}
}
