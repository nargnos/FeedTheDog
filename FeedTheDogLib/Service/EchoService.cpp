#include "EchoService.h"

namespace FeedTheDog
{

	EchoService::EchoService(int port):
		port_(port)
	{

	}


	EchoService::~EchoService()
	{
	}
	void EchoService::Run(TCore * core)
	{
		if (!acceptor)
		{
			acceptor = make_unique<_ASIO ip::tcp::acceptor>(core->GetIoService(), _ASIO ip::tcp::endpoint(_ASIO ip::tcp::v4(), port_));
		}
		StartAccept(core);
	}
	void EchoService::StartAccept(TCore * core)
	{
		auto& session = core->GetTcpSession();
		acceptor->async_accept(session->GetSocket(), _BOOST bind(&EchoService::HandleAccept, this, core, session, _ASIO placeholders::error));

	}
	void EchoService::ReadSome(shared_ptr<TTcpSession>& session)
	{
		session->GetSocket().async_read_some(
			_ASIO buffer(session->GetBuffer()),
			_BOOST bind(&EchoService::HandleRead, this, session, _ASIO placeholders::error, _ASIO placeholders::bytes_transferred)
			);
	}
	void EchoService::HandleAccept(TCore * core, shared_ptr<TTcpSession> session, const _BOOST system::error_code & error)
	{
		if (!error)
		{
			ReadSome(session);
		}
		StartAccept(core);
	}
	void EchoService::HandleRead(shared_ptr<TTcpSession> session, const _BOOST system::error_code & error, size_t bytes_transferred)
	{
		if (!error)
		{
			session->GetSocket().async_write_some(_ASIO buffer(session->GetBuffer(), bytes_transferred),
				_BOOST bind(&EchoService::HandleWrite, this, session, _ASIO placeholders::error)
				);
		}
	}

	void EchoService::HandleWrite(shared_ptr<TTcpSession> session, const _BOOST system::error_code & error)
	{
		if (!error)
		{
			ReadSome(session);
		}
	}
}  // namespace FeedTheDog


