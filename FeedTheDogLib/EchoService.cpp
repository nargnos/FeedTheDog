#include "stdafx.h"
#include "EchoService.h"

namespace FeedTheDog
{

	EchoService::EchoService(int port, const char* name) :
		ServiceBase(name),
		port_(port)
	{
	}


	EchoService::~EchoService()
	{
	}
	void EchoService::AsyncStart()
	{
		if (isStopped)
		{
			return;
		}
		auto& session = SelectIdleWorker()->NewSession<_ASIO ip::tcp>();
		AsyncAccept(*acceptor, *session, _BOOST bind(&EchoService::HandleAccept, this, session, _ASIO placeholders::error));

	}
	void EchoService::ReadSome(shared_ptr<TTcpSession>& session)
	{

		AsyncReadSome(*session,
			_ASIO buffer(session->GetBuffer()),
			_BOOST bind(&EchoService::HandleRead, this, session, _ASIO placeholders::error, _ASIO placeholders::bytes_transferred)
			);
	}
	void EchoService::HandleAccept(shared_ptr<TTcpSession>& session, const _BOOST system::error_code & error)
	{
		if (!error)
		{
			//auto& trace = manager->GetTrace();
			auto& endPoint = session->GetSocket().remote_endpoint();
			//	_STD ostringstream str;
			//	str << "Service " << name_ << ", New Connection: " << endPoint;
				//trace->TracePoint(str.str().c_str(), TraceLevel::Trace);
			ReadSome(session);
			AsyncStart();
		}
	}
	void EchoService::HandleRead(shared_ptr<TTcpSession>& session, const _BOOST system::error_code & error, size_t bytes_transferred)
	{
		if (!error)
		{
			AsyncWriteSome(*session, _ASIO buffer(session->GetBuffer(), bytes_transferred),
				_BOOST bind(&EchoService::HandleWrite, this, session, _ASIO placeholders::error)
				);
		}
	}

	void EchoService::HandleWrite(shared_ptr<TTcpSession>& session, const _BOOST system::error_code & error)
	{
		if (!error)
		{
			ReadSome(session);
		}
	}
	bool EchoService::InitService()
	{
		if (!acceptor)
		{
			auto& io = SelectIdleWorker()->GetIoService();
			acceptor = make_unique<_ASIO ip::tcp::acceptor>(io, _ASIO ip::tcp::endpoint(_ASIO ip::tcp::v4(), port_));

		}
		//manager->GetTrace()->TracePoint("Service Initialized", false, 0, name_, TraceLevel::Info);
		return true;
	}
	void EchoService::Stop()
	{
		//manager->GetTrace()->DebugPoint("stop echo");
		isStopped = true;
		acceptor->close();
	}
}  // namespace FeedTheDog


