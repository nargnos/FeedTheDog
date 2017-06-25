#ifndef ICONNECTION_H_
#define ICONNECTION_H_
#include "ITask.h"
#include "IFDTask.h"

namespace Detail
{
	class IConnection:
		public ITask,
		public IFDTask
	{
	};

}  // namespace Detail

#endif // ICONNECTION_H_

