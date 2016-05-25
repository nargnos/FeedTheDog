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
	bool EchoService::Init()
	{
		if (!acceptor)
		{
			auto& io = Impl()->GetIoService();
			acceptor = make_unique<_ASIO ip::tcp::acceptor>(io, _ASIO ip::tcp::endpoint(_ASIO ip::tcp::v4(), port_));

		}
		//manager->GetTrace()->TracePoint("Service Initialized", false, 0, name_, TraceLevel::Info);
		return true;
	}
	void EchoService::Start()
	{
		auto& session = Impl()->NewSession<Tcp, Pointer, NextWorker>(); // 手动控制对象析构并不会比智能指针快多少
		auto& socketRef = session->GetSocket();
		acceptor->async_accept(socketRef,
			[this, session_ = _STD move(session)](const _BOOST system::error_code & error) mutable
		{
			HandleAccept(session_, error);
		});

	}
	void EchoService::Stop()
	{
		//manager->GetTrace()->DebugPoint("stop echo");
		isStopped = true;
		acceptor->close();
	}

	void EchoService::ReadSome(TcpSessionPointer& session)
	{
		auto& buffer = session->GetBuffer();
		auto& socketRef = session->GetSocket();
		socketRef.async_read_some(
			_ASIO buffer(buffer),
			[session_ = _STD move(session)](const _BOOST system::error_code & error, size_t bytes_transferred) mutable
		{
			HandleRead(session_, error, bytes_transferred);
		});
	}
	void EchoService::HandleAccept(TcpSessionPointer& session, const _BOOST system::error_code & error)
	{
		if (!error)
		{
			Start();
			ReadSome(session);
			/*Impl()->PostTask([session]() mutable
			{
				ReadSome(session);
			});*/
		}
		else
		{
			session.Delete();
		}
	}
	void EchoService::HandleRead(TcpSessionPointer& session, const _BOOST system::error_code & error, size_t bytes_transferred)
	{
		if (!error)
		{
			auto& buffer = session->GetBuffer();
			auto& socketRef = session->GetSocket();
			_ASIO async_write(socketRef, _ASIO buffer(buffer, bytes_transferred),
				[session_ = _STD move(session)](const _BOOST system::error_code & error, size_t bytes_transferred) mutable
			{
				HandleWrite(session_, error);
			});
		}
		else
		{
			session.Delete();
		}
	}

	void EchoService::HandleWrite(TcpSessionPointer& session, const _BOOST system::error_code & error)
	{
		if (!error)
		{
			ReadSome(session);
		}
		else
		{
			session.Delete();
		}
	}

}  // namespace FeedTheDog


