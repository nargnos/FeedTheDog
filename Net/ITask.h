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
	// 执行完毕返回true，否则返回false等待下次执行
	virtual bool DoEvent(Loop& loop, std::unique_ptr<ITask>&& self)  = 0;
};


#endif // ITASK_H_

