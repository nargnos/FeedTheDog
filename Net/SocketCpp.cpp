#include "SocketCpp.h"
#include <utility>
namespace Detail
{

	Socket::Socket(int fd) :
		fd_(fd)
	{
	}

	Socket::Socket(Socket && sock)
	{
		*this = std::move(sock);
	}

	Socket & Socket::operator=(Socket && sock)
	{
		if (&sock == this)
		{
			return sock;
		}
		assert(fd_ == -1);
		fd_ = sock.fd_;
		sock.fd_ = -1;
		return *this;
	}


	Socket::operator bool() const
	{
		return fd_ != -1;
	}

	ssize_t Socket::Read(void * buf, size_t nBytes) const
	{
		assert(fd_ != -1);
		assert(buf != nullptr && nBytes > 0);
		return read(fd_, buf, nBytes);
	}

	ssize_t Socket::Readv(const iovec * iov, int count) const
	{
		assert(fd_ != -1);
		assert(iov != nullptr && count > 0);
		return readv(fd_, iov, count);
	}

	ssize_t Socket::Writev(const iovec * iov, int count) const
	{
		assert(fd_ != -1);
		assert(iov != nullptr && count > 0);
		return writev(fd_, iov, count);
	}

	ssize_t Socket::Write(const void * buf, size_t nBytes) const
	{
		assert(fd_ != -1);
		assert(buf != nullptr && nBytes > 0);
		return write(fd_, buf, nBytes);
	}

	bool Socket::Bind(const sockaddr_in & addr) const
	{
		assert(fd_ != -1);
		return bind(fd_, (sockaddr*)&addr, sizeof(sockaddr_in)) != -1;
	}

	bool Socket::Connect(const sockaddr_in & addr) const
	{
		assert(fd_ != -1);
		return connect(fd_, (sockaddr*)&addr, sizeof(sockaddr_in)) != -1;
	}

	bool Socket::SetNonBlocking() const
	{
		assert(fd_ != -1);
		int cflags = fcntl(fd_, F_GETFL, 0);
		if (__glibc_unlikely(cflags < 0))
		{
			return false;
		}
		return fcntl(fd_, F_SETFL, cflags | O_NONBLOCK) != -1;
	}
	bool Socket::IsNonBlocking() const
	{
		assert(fd_ != -1);
		int cflags = fcntl(fd_, F_GETFL, 0);
		if (__glibc_unlikely(cflags < 0))
		{
			return false;
		}
		return cflags & O_NONBLOCK;
	}
	bool Socket::SetSocketOption(SocketLevel level, OptName opt, const void * optval, socklen_t len) const
	{
		assert(fd_ != -1);
		return setsockopt(fd_, level, opt, optval, len) != -1;
	}

	bool Socket::GetSocketOption(SocketLevel level, OptName opt, void * __restrict optval, socklen_t * __restrict len) const
	{
		assert(fd_ != -1);
		return getsockopt(fd_, level, opt, optval, len) != -1;
	}

	bool Socket::SetReuseAddr(bool opt) const
	{
		int val = opt ? 1 : 0;
		return SetSocketOption(SocketLevel::SolSocket,
			OptName::Opt_ReuseAddr,
			&val,
			sizeof(val));
	}

	bool Socket::SetReusePort(bool opt) const
	{
		int val = opt ? 1 : 0;
		return SetSocketOption(SocketLevel::SolSocket,
			OptName::Opt_ReusePort,
			&val,
			sizeof(val));
	}

	void Socket::Close()
	{
		if (fd_ == -1)
		{
			return;
		}
		close(fd_);
		fd_ = -1;
	}

	Socket::~Socket()
	{
		if (fd_ != -1)
		{
			close(fd_);
		}
	}

	int Socket::FD() const
	{
		return fd_;
	}

	TcpSocket::TcpSocket() :
		Socket(socket(AF_INET, SOCK_STREAM, 0))
	{
	}

	TcpSocket::TcpSocket(int fd) :
		Socket(fd)
	{

	}

	bool TcpSocket::Listen() const
	{
		assert(fd_ != -1);
		return listen(fd_, SOMAXCONN) != -1;
	}

	int TcpSocket::Accept(sockaddr_in & __restrict addr, socklen_t & __restrict len) const
	{
		assert(fd_ != -1);
		return accept(fd_, (sockaddr* __restrict)&addr, &len);
	}

	int TcpSocket::Accept() const
	{
		assert(fd_ != -1);
		return accept(fd_, nullptr, nullptr);
	}

	bool TcpSocket::Shutdown(ShutdownMode how) const
	{
		assert(fd_ != -1);
		return shutdown(fd_, how) != -1;
	}

	bool TcpSocket::SetKeepAlive(bool opt) const
	{
		int val = opt ? 1 : 0;
		return SetSocketOption(SocketLevel::SolSocket,
			OptName::Opt_KeepAlive,
			&val,
			sizeof(val));
	}

	bool TcpSocket::SetNoDelay(bool opt) const
	{
		int val = opt ? 1 : 0;
		return SetSocketOption(SocketLevel::IPProtoTcp,
			OptName::Opt_TcpNoDelay,
			&val,
			sizeof(val));
	}

	UdpSocket::UdpSocket() :Socket(socket(AF_INET, SOCK_DGRAM, 0))
	{
	}

	ssize_t UdpSocket::SendTo(const void * buf, size_t nBytes,
		int flags, const sockaddr_in & addr) const
	{
		return sendto(fd_, buf, nBytes, flags, (const sockaddr*)&addr, sizeof(sockaddr_in));
	}

	ssize_t UdpSocket::RecvFrom(void * __restrict buf, size_t nBytes,
		int flags, sockaddr_in * __restrict addr, socklen_t * __restrict size) const
	{
		return recvfrom(fd_, buf, nBytes, flags, (sockaddr* __restrict)&addr, size);
	}


	// addr参数在函数中转换字节序
	sockaddr_in MakeSockaddr(in_addr_t addr, uint16_t port)
	{
		sockaddr_in result;
		result.sin_family = AF_INET;
		result.sin_addr.s_addr = htonl(addr);
		result.sin_port = htons(port);
		return result;

	}

	sockaddr_in MakeSockaddr(const char * addr, uint16_t port)
	{
		sockaddr_in result;
		result.sin_family = AF_INET;
		result.sin_port = htons(port);
		result.sin_addr.s_addr = inet_addr(addr);
		return result;
	}

}  // namespace Detail