#ifndef FDTASKCTLATTORNEY_H_
#define FDTASKCTLATTORNEY_H_
#include "EpollOption.h"
#include "IFDTask.h"
class Loop;
class FDTaskCtlAttorney
{
public:
	static void Add(Loop& loop, EpollOption op, IFDTask* obj);
	static void Mod(Loop& loop, EpollOption op, IFDTask* obj);
	static void Del(Loop& loop, IFDTask* obj);

private:
	FDTaskCtlAttorney() = delete;
	~FDTaskCtlAttorney() = delete;
};

#endif // FDTASKCTLATTORNEY_H_

