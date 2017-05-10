#ifndef EVENTTASK_H_
#define EVENTTASK_H_
#include "IFDTask.h"
#include "EventFd.h"
#include "Noncopyable.h"

namespace Detail
{
	class EventTaskBase :
		public Noncopyable,
		public IFDTask
	{
	public:
		EventTaskBase(unsigned int count, EventFdFlags flags);
		virtual int FD() const;
	protected:
		EventFd fd_;
	};
}  // namespace Detail

#endif // EVENTTASK_H_


