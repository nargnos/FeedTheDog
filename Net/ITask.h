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
	// ִ����Ϸ���true�����򷵻�false�ȴ��´�ִ��
	virtual bool DoEvent(Loop& loop, std::unique_ptr<ITask>&& self)  = 0;
};


#endif // ITASK_H_
