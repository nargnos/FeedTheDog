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
		virtual void AsyncStart() override;
		virtual void Stop() override;
		
	private:
		int port_;
		virtual bool InitService() override;
		void ReadSome(shared_ptr<TTcpSession>&);
		void HandleAccept(shared_ptr<TTcpSession>&,const _BOOST system::error_code& error);
		void HandleRead(shared_ptr<TTcpSession>&, const _BOOST system::error_code& error, size_t bytes_transferred);

		void HandleWrite(shared_ptr<TTcpSession>& session, const _BOOST system::error_code & error);

		unique_ptr<_ASIO ip::tcp::acceptor> acceptor;

	};
}  // namespace FeedTheDog
