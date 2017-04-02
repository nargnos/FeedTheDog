#include "EventTaskBase.h"

EventTaskBase::EventTaskBase(unsigned int count, EventFdFlags flags) :
	fd_(count, flags)
{
}

int EventTaskBase::FD() const
{
	return fd_.FD();
}
