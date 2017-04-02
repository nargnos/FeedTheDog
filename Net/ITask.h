#ifndef ITASK_H_
#define ITASK_H_
#include <memory>
#include <type_traits>
#include "EpollOption.h"
class Loop;
class ITask
{
public:
	ITask() = default;
	virtual ~ITask() = default;
	// 如果要重复执行同一个任务，需要重新添加回队列
	virtual void DoEvent(Loop& loop, std::unique_ptr<ITask>&& self)  = 0;
};


#endif // ITASK_H_

