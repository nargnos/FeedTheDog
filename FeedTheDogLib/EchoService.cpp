#include "stdafx.h"
#include "EchoService.h"

namespace FeedTheDog
{

	EchoService::EchoService(int port, const char* name) :
		ServiceBase(name),
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
		
		auto& session = manager->SelectIdleWorker()->NewSession<_ASIO ip::tcp>();
		acceptor->async_accept(*session, _BOOST bind(&EchoService::HandleAccept, this, session, _ASIO placeholders::error));

	}
	void EchoService::ReadSome(shared_ptr<TTcpSession>& session)
	{

		session->async_read_some(
			_ASIO buffer(session->GetBuffer()),
			_BOOST bind(&EchoService::HandleRead, this, session, _ASIO placeholders::error, _ASIO placeholders::bytes_transferred)
			);
	}
	void EchoService::HandleAccept(shared_ptr<TTcpSession>& session, const _BOOST system::error_code & error)
	{
		if (!error)
		{
			//auto& trace = manager->GetTrace();
			auto& endPoint = session->remote_endpoint();
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
			session->async_write_some(_ASIO buffer(session->GetBuffer(), bytes_transferred),
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
	bool EchoService::Init(TServiceManager* corePtr)
	{
		isStop = false;
		manager = corePtr;

		if (!acceptor)
		{
			auto& io = corePtr->SelectIdleWorker()->GetIoService();
			acceptor = make_unique<_ASIO ip::tcp::acceptor>(io, _ASIO ip::tcp::endpoint(_ASIO ip::tcp::v4(), port_));

		}
		//manager->GetTrace()->TracePoint("Service Initialized", false, 0, name_, TraceLevel::Info);
		return true;
	}
	void EchoService::Stop()
	{
		//manager->GetTrace()->DebugPoint("stop echo");
		isStop = true;
		acceptor->close();
	}
}  // namespace FeedTheDog


