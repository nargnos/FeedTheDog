#include "TcpAttachAttorney.h"
#include "TcpProactorConnection.h"
namespace Detail
{
	std::shared_ptr<TcpProactorConnection> TcpAttachAttorney::Attach(Loop & loop, int fd)
	{
		assert(fd != -1);
		return TcpProactorConnection::Attach(loop, fd);
	}

}  // namespace Detail