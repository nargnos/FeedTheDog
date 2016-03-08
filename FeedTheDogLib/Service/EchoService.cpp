#include "EchoService.h"

namespace FeedTheDog
{

	EchoService::EchoService(int port) :
		port_(port)
	{
		isStop = false;
	}


	EchoService::~EchoService()
	{
	}
	void EchoService::AsyncStart()
	{
		if (isStop)
		{
			return;
		}
		auto& session = core->GetIdleSessionPool()->NewSession<_ASIO ip::tcp>();
		acceptor->async_accept(session->GetSocket(), _BOOST bind(&EchoService::HandleAccept, this, session, _ASIO placeholders::error));

	}
	void EchoService::ReadSome(shared_ptr<TTcpSession>& session)
	{
		session->GetSocket().async_read_some(
			_ASIO buffer(session->GetBuffer()),
			_BOOST bind(&EchoService::HandleRead, this, session, _ASIO placeholders::error, _ASIO placeholders::bytes_transferred)
			);
	}
	void EchoService::HandleAccept(shared_ptr<TTcpSession> session, const _BOOST system::error_code & error)
	{
		if (!error)
		{
			ReadSome(session);
		}
		AsyncStart();
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
	bool EchoService::Init(TCore* corePtr)
	{
		isStop = false;
		core = corePtr;

		if (!acceptor)
		{
			auto& io = corePtr->GetIdleSessionPool()->GetIoService();
			acceptor = make_unique<_ASIO ip::tcp::acceptor>(io, _ASIO ip::tcp::endpoint(_ASIO ip::tcp::v4(), port_));
			acceptor->listen();
		}
		return true;
	}
	void EchoService::Stop()
	{
		isStop = true;
	}
}  // namespace FeedTheDog


