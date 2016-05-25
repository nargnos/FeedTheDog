#pragma once
#include "ServiceBase.h"
namespace FeedTheDog
{
	class EchoService :
		public ServiceBase
	{
	public:
		EchoService(int port, const char* name);
		~EchoService();
		virtual void Start() override;
		virtual void Stop() override;

	private:

		virtual bool Init() override;

		void HandleAccept(TcpSessionPointer&, const _BOOST system::error_code& error);
		static void HandleRead(TcpSessionPointer&, const _BOOST system::error_code& error, size_t bytes_transferred);
		static void ReadSome(TcpSessionPointer&);
		static void HandleWrite(TcpSessionPointer& session, const _BOOST system::error_code & error);

		unique_ptr<Tcp::acceptor> acceptor;
		int port_;
		bool isStopped;

	};
}  // namespace FeedTheDog
