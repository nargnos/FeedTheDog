#ifndef ITCPACCEPTOR_H_
#define ITCPACCEPTOR_H_
#include "IFDTask.h"
class Loop;
class ITcpAcceptor :public IFDTask
{
public:
	ITcpAcceptor() = default;

	virtual ~ITcpAcceptor() = default;
	// 交换给另一个线程调用的回调
	virtual void DoAccept(Loop& loop, int fd) = 0;
};

#endif // ITCPACCEPTOR_H_

