#include "stdafx.h"
#include "EchoService.h"

namespace FeedTheDog
{

	EchoService::EchoService(int port, const char* name) :
		ServiceBase(name),
		port_(port),
		isStopped(false)
	{
	}


	EchoService::~EchoService()
	{
	}
	void EchoService::Start()
	{
		if (isStopped)
		{
			return;
		}
		auto& session = NewSession<_ASIO ip::tcp>();
		auto& socketRef = session->GetSocket();
		acceptor->async_accept(socketRef,
			[this, session_ = _STD move(session)](const _BOOST system::error_code & error) mutable
		{
			HandleAccept(session_, error);
		});

	}
	void EchoService::ReadSome(shared_ptr<TTcpSession>& session)
	{
		auto& buffer = session->GetBuffer();
		auto& sessionRef = *session;
		sessionRef.AsyncReadSome(
			_ASIO buffer(buffer),
			[this, session_ = _STD move(session)](const _BOOST system::error_code & error, size_t bytes_transferred) mutable
		{
			HandleRead(session_, error, bytes_transferred);
		});
	}
	void EchoService::HandleAccept(shared_ptr<TTcpSession>& session, const _BOOST system::error_code & error)
	{
		if (!error)
		{
			//auto& trace = manager->GetTrace();
			//auto& endPoint = session->GetSocket().remote_endpoint();
			//	_STD ostringstream str;
			//	str << "Service " << name_ << ", New Connection: " << endPoint;
				//trace->TracePoint(str.str().c_str(), TraceLevel::Trace);
			ReadSome(session);
			Start();
		}
	}
	void EchoService::HandleRead(shared_ptr<TTcpSession>& session, const _BOOST system::error_code & error, size_t bytes_transferred)
	{
		if (!error)
		{
			auto& buffer = session->GetBuffer();
			auto& sessionRef = *session;
			sessionRef.AsyncWrite(_ASIO buffer(buffer, bytes_transferred),
				[this, session_ = _STD move(session)](const _BOOST system::error_code & error, size_t bytes_transferred) mutable
			{
				HandleWrite(session_, error);
			});
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


