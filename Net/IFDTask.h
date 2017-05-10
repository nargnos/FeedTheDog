#ifndef IFDTASK_H_
#define IFDTASK_H_
#include "EpollOption.h"
namespace Detail
{
	class Loop;
	class IFDTask
	{
	public:
		IFDTask() = default;
		virtual ~IFDTask() = default;
		virtual void DoEvent(Loop& loop, EpollOption op) = 0;
		virtual int FD() const = 0;
	};
}  // namespace Detail
#endif // IFDTASK_H_

