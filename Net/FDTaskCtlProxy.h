#ifndef FDTASKCTLPROXY_H_
#define FDTASKCTLPROXY_H_
#include "EpollOption.h"
#include "IFDTask.h"
class Loop;
class FDTaskCtlProxy
{
public:
	static void Add(Loop& loop, EpollOption op, IFDTask* obj);
	static void Mod(Loop& loop, EpollOption op, IFDTask* obj);
	static void Del(Loop& loop, IFDTask* obj);

private:
	FDTaskCtlProxy() = delete;
	~FDTaskCtlProxy() = delete;
};

#endif // FDTASKCTLPROXY_H_

