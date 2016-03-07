#pragma once
#include "ServiceBase.h"
namespace FeedTheDog
{
	class EchoService :
		public ServiceBase<0>
	{
	public:
		EchoService(int port);
		~EchoService();
		virtual void Run(TCore *) override;
	private:
		int port_;
		void StartAccept(TCore *);
		void ReadSome(shared_ptr<TTcpSession>&);
		void HandleAccept(TCore *, shared_ptr<TTcpSession>,const _BOOST system::error_code& error);
		void HandleRead(shared_ptr<TTcpSession>, const _BOOST system::error_code& error, size_t bytes_transferred);

		void HandleWrite(shared_ptr<TTcpSession> session, const _BOOST system::error_code & error);

		unique_ptr<_ASIO ip::tcp::acceptor> acceptor;
	};
}  // namespace FeedTheDog
