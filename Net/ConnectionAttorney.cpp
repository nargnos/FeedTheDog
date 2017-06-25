#include "ConnectionAttorney.h"
#include "ProactorConnection.h"
#include "TcpProactorConnection.h"
#include "SocketAttorney.h"
void Detail::ConnectionAttorney::Recycle(ProactorConnection & conn)
{
	conn.Recycle();
}

void Detail::ConnectionAttorney::Reset(TcpProactorConnection& conn,Loop& loop, int fd)
{
	assert(fd != -1);
	assert(&conn.loop_ == &loop);
	SocketAttorney::SetFd(conn.socket_, fd);
	assert(conn.socketPtr_ == nullptr);
	conn.socketPtr_ = &conn.socket_;
	conn.SetNonBlocking();
}
