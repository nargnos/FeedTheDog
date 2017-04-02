
#ifndef EVENTFD_H_
#define EVENTFD_H_
#include <sys/eventfd.h>
#include "Noncopyable.h"
enum EventFdFlags:int
{
	None = 0,
	CloseOnExec = EFD_CLOEXEC,
	Nonblock = EFD_NONBLOCK,
	Semaphore = EFD_SEMAPHORE
};


class EventFd :public Noncopyable
{
public:
	EventFd(unsigned int count, EventFdFlags flags = EventFdFlags::None);
	~EventFd();
	int FD()const;
	bool Read(eventfd_t* val)const;
	bool Write(eventfd_t val)const;
private:
	int fd_;
};

#endif // EVENTFD_H_


