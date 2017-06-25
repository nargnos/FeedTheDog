#ifndef SOCKETATTORNEY_H_
#define SOCKETATTORNEY_H_


namespace Detail
{
	class Socket;
	class SocketAttorney
	{
	public:
		static void SetFd(Socket& sock, int newfd);
	private:
		SocketAttorney() = delete;

		~SocketAttorney() = delete;
	};

}  // namespace Detail

#endif // SOCKETATTORNEY_H_

