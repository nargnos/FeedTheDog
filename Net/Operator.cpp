#include "Operator.h"
#include <errno.h>
#include <sys/epoll.h>
void Detail::SetErrno(Error & err)
{
	err.Errno = errno;
	err.State = LIKELY(IsContinue(err.Errno)) ? ErrStat::Pause : ErrStat::Error;
}
void Detail::SetErrStat(Error & err, ErrStat s)
{
	err.State = s;
}
bool Detail::IsContinue(int err)
{
	return err == EAGAIN || err == EWOULDBLOCK || err == EINTR;
}

void Detail::Operator::DoOperator(Error & error)
{
	operator_(this, error);
}

void Detail::Operator::DoComplete(Error e)
{
	complete_(this, e);
}
