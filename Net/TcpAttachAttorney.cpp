#include "TcpAttachAttorney.h"
#include "TcpProactorConnection.h"
std::shared_ptr<TcpProactorConnection> TcpAttachAttorney::Attach(Loop & loop, int fd)
{
	return TcpProactorConnection::Attach(loop, fd);
}
