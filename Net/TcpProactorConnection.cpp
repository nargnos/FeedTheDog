#include "TcpProactorConnection.h"
#include "ConnectionAttorney.h"
#include "TlsPackage.h"
Detail::TcpProactorConnection::TcpProactorConnection(Loop & loop, int fd) :
	ProactorConnection(loop, &socket_),
	socket_(fd)
{
	SetNonBlocking();
}

std::shared_ptr<Detail::TcpProactorConnection> Detail::TcpProactorConnection::Attach(Loop & loop, int fd)
{
	return GlobalTlsPackage::Instance().TlsConnPool.GetTcpConn(loop, fd);
}

bool Detail::TcpProactorConnection::SetKeepAlive(bool opt) const
{
	return socket_.SetKeepAlive(opt);
}

bool Detail::TcpProactorConnection::SetNoDelay(bool opt) const
{
	return socket_.SetNoDelay(opt);
}

Detail::TcpSocket & Detail::TcpProactorConnection::Socket()
{
	return socket_;
}

