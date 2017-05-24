#include "TcpProactorConnection.h"

Detail::TcpProactorConnection::TcpProactorConnection(Loop & loop, int fd) :
	ProactorConnection(loop),
	socket_(fd)
{
	SetNonBlocking();
}

std::shared_ptr<Detail::TcpProactorConnection> Detail::TcpProactorConnection::Attach(Loop & loop, int fd)
{
	return std::shared_ptr<TcpProactorConnection>(new TcpProactorConnection(loop, fd));
}

int Detail::TcpProactorConnection::FD() const
{
	return socket_.FD();
}

Detail::TcpSocket & Detail::TcpProactorConnection::Socket()
{
	return socket_;
}

void Detail::TcpProactorConnection::SetNonBlocking()
{
	if (!socket_.SetNonBlocking())
	{
		TRACEPOINT(LogPriority::Warning)("tcpsocket setnonblocking failed");
	}
}
