#ifndef SOCKETCPP_H_
#define SOCKETCPP_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include "Noncopyable.h"
namespace Detail
{

	enum SocketLevel
	{
		SolSocket = SOL_SOCKET, // 传输层
		IPProtoIP = IPPROTO_IP, // 网络层
		IPProtoTcp = IPPROTO_TCP
	};
	enum OptName
	{
		// bool
		Opt_AcceptConn = SO_ACCEPTCONN,
		Opt_Broadcast = SO_BROADCAST,
		Opt_Debug = SO_DEBUG,
		Opt_DontRoute = SO_DONTROUTE,
		Opt_KeepAlive = SO_KEEPALIVE,
		Opt_OOBInline = SO_OOBINLINE,
		Opt_ReuseAddr = SO_REUSEADDR,
		Opt_ReusePort = SO_REUSEPORT,
		Opt_TcpNoDelay = TCP_NODELAY,

		// int
		Opt_Error = SO_ERROR,
		Opt_RcvBuf = SO_RCVBUF,
		Opt_Type = SO_TYPE,
		Opt_SndTimeo = SO_SNDTIMEO,
		Opt_RcvTimeo = SO_RCVTIMEO,
		Opt_SndBuf = SO_SNDBUF,

		Opt_Linger = SO_LINGER,

	};
	class Socket :
		public Noncopyable
	{
	public:
		explicit Socket(int fd);
		explicit operator bool() const;
		ssize_t Read(void* buf, size_t nBytes) const;
		ssize_t Readv(const iovec* iov, int count) const;
		ssize_t Writev(const iovec* iov, int count) const;
		ssize_t Write(const void* buf, size_t nBytes) const;
		bool Bind(const sockaddr_in& addr) const;
		bool Connect(const sockaddr_in& addr) const;
		bool SetSocketOption(SocketLevel level, OptName opt, const void* optval, socklen_t len) const;
		bool GetSocketOption(SocketLevel level, OptName opt, void* __restrict optval, socklen_t* __restrict len) const;

		bool SetNoDelay(bool opt) const;
		bool SetNonBlocking() const;
		bool IsNonBlocking() const;
		bool SetReuseAddr(bool opt) const;
		bool SetReusePort(bool opt) const;
		bool SetKeepAlive(bool opt) const;
		void Close();
		virtual ~Socket();
		int FD() const;
	protected:
		int fd_;
	};
	enum ShutdownMode
	{
		Read = SHUT_RD,
		Write = SHUT_WR,
		Both = SHUT_RDWR
	};
	class TcpSocket :
		public Socket
	{
	public:
		TcpSocket();
		explicit TcpSocket(int fd);
		bool Listen() const;
		int Accept(sockaddr_in& __restrict addr, socklen_t& __restrict len) const;
		int Accept() const;
		bool Shutdown(ShutdownMode how) const;
	};
	class UdpSocket :
		public Socket
	{
	public:
		UdpSocket();

		ssize_t SendTo(const void* buf, size_t nBytes, int flags, const sockaddr_in& addr) const;
		ssize_t RecvFrom(void*__restrict buf, size_t nBytes, int flags, sockaddr_in*__restrict addr, socklen_t*__restrict size) const;
	};
	// addr参数在函数中转换字节序
	sockaddr_in MakeSockaddr(in_addr_t addr, uint16_t port);
	sockaddr_in MakeSockaddr(const char* addr, uint16_t port);
}  // namespace Detail
using Detail::MakeSockaddr;

#endif // SOCKETCPP_H_

