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
		bool isStopped;
		int port_;
		virtual bool InitService() override;
		void FASTCALL ReadSome(shared_ptr<TcpSession>&);
		void FASTCALL HandleAccept(shared_ptr<TcpSession>&,const _BOOST system::error_code& error);
		void FASTCALL HandleRead(shared_ptr<TcpSession>&, const _BOOST system::error_code& error, size_t bytes_transferred);

		void FASTCALL HandleWrite(shared_ptr<TcpSession>& session, const _BOOST system::error_code & error);

		unique_ptr<_ASIO ip::tcp::acceptor> acceptor;

	};
}  // namespace FeedTheDog
