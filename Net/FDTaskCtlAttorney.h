#ifndef FDTASKCTLATTORNEY_H_
#define FDTASKCTLATTORNEY_H_
#include "EpollOption.h"
#include "IFDTask.h"

namespace Detail
{
	class Loop;
	class FDTaskCtlAttorney
	{
	public:
		static void Add(const Loop& loop, EpollOption op, const IFDTask* obj);
		static void Mod(const Loop& loop, EpollOption op, const IFDTask* obj);
		static void Del(const Loop& loop, const IFDTask* obj);

	private:
		FDTaskCtlAttorney() = delete;
		~FDTaskCtlAttorney() = delete;
	};
}  // namespace Detail

#endif // FDTASKCTLATTORNEY_H_

