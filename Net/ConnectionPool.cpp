#include "ConnectionPool.h"
#include "TcpProactorConnection.h"
#include "ConnectionAttorney.h"
namespace Detail
{
	ConnectionPool::TcpConnPtr ConnectionPool::GetTcpConn(Loop & loop, int fd)
	{
		TcpProactorConnection* ret;

		if (!tcp_.empty())
		{
			ret = tcp_.top().release();
			tcp_.pop();
			assert(ret != nullptr);
			ConnectionAttorney::Reset(*ret, loop, fd);
		}
		else
		{
			ret = new TcpProactorConnection(loop, fd);
		}
		return TcpConnPtr(ret, [this](TcpProactorConnection* ptr)
		{
			if (tcp_.size() < MaxSize)
			{
				ConnectionAttorney::Recycle(*ptr);
				tcp_.push(TcpConnUniPtr(ptr));
				return;
			}
			delete ptr;
		});
	}

}  // namespace Detail