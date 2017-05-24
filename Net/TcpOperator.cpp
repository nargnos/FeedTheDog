#include "TcpOperator.h"

void Detail::AddProgress(Progress & p, size_t trans, Error & e)
{
	if (p.Step(trans) == 0)
	{
		SetErrStat(e, ErrStat::Success);
	}
	else
	{
		SetErrStat(e, ErrStat::Continue);
	}
}
