#include "SocketAttorney.h"
#include "SocketCpp.h"

void Detail::SocketAttorney::SetFd(Socket & sock, int newfd)
{
	assert(sock.fd_ == -1);
	sock.fd_ = newfd;
}
