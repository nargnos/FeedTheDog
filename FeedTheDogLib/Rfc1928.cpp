#include "stdafx.h"
#include "Rfc1928.h"
namespace FeedTheDog
{
	Rfc1928::Rfc1928(int port, const char* name, int timeout)
		:ServiceBase(name),
		port_(port),
		timeoutSecond(timeout),
		isStopped(false)
	{
	}

	Rfc1928::~Rfc1928()
	{
	}

	bool Rfc1928::InitService()
	{
		if (!acceptor)
		{
			auto& io = SelectIdleWorker()->GetIoService();
			// ipv4后面再改
			acceptor = make_unique<Tcp::acceptor>(io,
				Tcp::endpoint(Tcp::v4(), port_));
			// 设置支持的函数
			memset(supportMethods, false, 0xff);
			// FIX: 需要从配置读取或用策略传入
			supportMethods[Rfc1928::NoAuthenticationRequired] = true;
			// gssapi
			// username & password
			// 0x03 - 0x7f iana
			// 0x80 - 0xfe reserved
		}
		return true;
	}

	void Rfc1928::Start()
	{
		if (isStopped)
		{
			return;
		}
		GetTrace()->DebugPoint(__func__);
		auto& session = NewSession<Tcp>();
		// buffer建议大于1k
		assert(session->GetBuffer().max_size() > 1024);
		auto& deadlineSession = make_shared<TTcpDeadlineSession>(_STD move(session));

		deadlineSession->GetTimer().async_wait(
			[this, deadlineSession](const _BOOST system::error_code& error) mutable {
			CheckDeadline(deadlineSession, error);
		});
		auto& tmpSession = deadlineSession->GetSession();
		acceptor->async_accept(tmpSession->GetSocket(),
			[this, ptr = _STD move(deadlineSession)](const _BOOST system::error_code & error) mutable {
			HandleAccept(ptr, error);
		});
	}

	void Rfc1928::CheckDeadline(shared_ptr<TTcpDeadlineSession>& deadlineSession,
		const _BOOST system::error_code& error)
	{
		GetTrace()->DebugPoint(__func__);
		if (error || isStopped)
		{
			return;
		}
		auto& timer = deadlineSession->GetTimer();
		if (timer.expires_at() <= _ASIO deadline_timer::traits_type::now())
		{
			deadlineSession->GetSession()->Close(ignore);
			return;
		}
		timer.async_wait(
			[this, ptr = _STD move(deadlineSession)](const _BOOST system::error_code& error) mutable {
			CheckDeadline(ptr, error);
		});
	}

	void Rfc1928::Stop()
	{
		GetTrace()->DebugPoint("stop", false, 0, Name());
		isStopped = true;
		acceptor->close();
	}

	void Rfc1928::HandleAccept(shared_ptr<TTcpDeadlineSession>& deadlineSession,
		const _BOOST system::error_code & error)
	{
		GetTrace()->DebugPoint(__func__);
		if (error || isStopped)
		{
			deadlineSession->Close(ignore);
			return;
		}
		deadlineSession->GetTimer().expires_from_now(timeoutSecond);
		auto& session = deadlineSession->GetSession();

		session->AsyncReadSome(_ASIO buffer(session->GetBuffer(), VersionMessage::MaxSize),
			[this, ptr = _STD move(deadlineSession)](const _BOOST system::error_code & error, size_t bytes_transferred) mutable {
			HandleReadVersionMessage(ptr, 0, bytes_transferred, error);
		});
		Start();
	}

	void Rfc1928::HandleReadVersionMessage(shared_ptr<TTcpDeadlineSession>& deadlineSession,
		size_t alreadyTransferred, size_t bytes_transferred, const _BOOST system::error_code & error)
	{
		GetTrace()->DebugPoint(__func__);
		if (error || isStopped)
		{
			deadlineSession->Close(ignore);
			return;
		}
		auto& session = deadlineSession->GetSession();
		auto& timer = deadlineSession->GetTimer();
		timer.expires_at(_BOOST posix_time::pos_infin);
		alreadyTransferred += bytes_transferred;

		auto& buffer = session->GetBuffer();
		auto bufferData = buffer.data();

		auto vmHeader = reinterpret_cast<VersionMessage*>(bufferData);
		if (vmHeader->Ver != VersionMessage::VerValue)
		{
			deadlineSession->Close(ignore);
			return;
		}
		if (!CheckVersionMessage(alreadyTransferred, vmHeader))
		{
			timer.expires_from_now(timeoutSecond);

			TcpReadMore(session,
				[this, ptr = _STD move(deadlineSession), alreadyTransferred]
			(const _BOOST system::error_code & error, size_t bytes_transferred) mutable
			{
				HandleReadVersionMessage(ptr, alreadyTransferred, bytes_transferred, error);
			}, alreadyTransferred, (size_t)vmHeader->NMethods + VersionMessage::FirstTwoBytes);
			return;
		}
		// 读取到了正确长度，协商过程未结束，客户端多发的作废
		ReplySelectedMethod(deadlineSession, vmHeader);

	}

	bool Rfc1928::CheckVersionMessage(size_t alreadyTransferred, const VersionMessage* vmHeader)
	{
		return alreadyTransferred >= ((size_t)vmHeader->NMethods + VersionMessage::FirstTwoBytes);
	}

	// 返回选择的函数
	void Rfc1928::ReplySelectedMethod(shared_ptr<TTcpDeadlineSession>& deadlineSession, VersionMessage* vmHeader)
	{
		GetTrace()->DebugPoint(__func__);
		auto& session = deadlineSession->GetSession();
		unsigned char nmethods = vmHeader->NMethods;
		// 使用原本的buffer存储返回信息
		auto replyBuffer = reinterpret_cast<MethodSelectionMessage*>(vmHeader);
		replyBuffer->Method = MethodSelectionMessage::NoAcceptableMethods;
		// 选择函数
		for (size_t i = 0; i < nmethods; i++)
		{
			auto tmpMethod = vmHeader->Methods[i];
			if (supportMethods[tmpMethod])
			{
				replyBuffer->Method = tmpMethod;
				break;
			}
		}
		assert(replyBuffer->Ver = MethodSelectionMessage::VerValue);
		// 返回给客户端
		session->AsyncWrite(_ASIO buffer(replyBuffer, sizeof(MethodSelectionMessage)),
			[this, ptr = _STD move(deadlineSession), method = replyBuffer->Method]
		(const _BOOST system::error_code & error, size_t bytes_transferred) mutable
		{
			if (!error)
			{
				RunMethod(ptr, method);
			}
		});
	}

	void Rfc1928::RunMethod(shared_ptr<TTcpDeadlineSession>& deadlineSession, unsigned char selectedMethod)
	{
		GetTrace()->DebugPoint(__func__);
		auto& timer = deadlineSession->GetTimer();
		auto& session = deadlineSession->GetSession();
		switch (selectedMethod)
		{
		case NoAuthenticationRequired:
			timer.expires_from_now(timeoutSecond);

			TcpReadMore(session,
				[this, ptr = _STD move(deadlineSession)]
			(const _BOOST system::error_code & error, size_t bytes_transferred) mutable{
				HandleNoAuthenticationRequired(ptr, 0, bytes_transferred, error);
			});
			return;
		case Gssapi:
			// TODO:
			return;
		case UsernamePassword:
			// TODO:
			return;
		default:
			if (selectedMethod >= IanaBegin && selectedMethod <= IanaEnd)
			{
				// iana
				// TODO:
			}
			else if (selectedMethod >= ReservedBegin && selectedMethod <= ReservedEnd)
			{
				// reserved
				// TODO:
			}
			else
			{
				// ff
				deadlineSession->Close(ignore);
			}
			return;
		}
		__assume(false);
	}

	// FIX: 函数过长
	void Rfc1928::HandleNoAuthenticationRequired(shared_ptr<TTcpDeadlineSession>& deadlineSession,
		size_t alreadyTransferred, size_t bytes_transferred, const _BOOST system::error_code & error)
	{
		GetTrace()->DebugPoint(__func__);
		if (isStopped || error)
		{
			deadlineSession->Close(ignore);
			return;
		}
		auto& session = deadlineSession->GetSession();
		auto& timer = deadlineSession->GetTimer();
		timer.expires_at(_BOOST posix_time::pos_infin);
		alreadyTransferred += bytes_transferred;
		auto& buffer = session->GetBuffer();
		auto bufferData = buffer.data();

		auto requestDetail = reinterpret_cast<ClientRequestDetailMessage*>(bufferData);
		// 地址数据开始位置
		size_t requestLen = sizeof(ClientRequestDetailMessage);
		auto addrPtr = bufferData + requestLen;

		// 假设读取足够长度，预先判断标识
		if (requestDetail->Ver != ClientRequestDetailMessage::VerValue ||
			requestDetail->Rsv != ClientRequestDetailMessage::RsvValue)
		{
			deadlineSession->Close(ignore);
			return;
		}


		// 假设读取足够长度，预先处理aytp
		switch (requestDetail->Atyp)
		{
		case ClientRequestDetailMessage::AtypIPv4:
		{
			requestLen += ClientRequestDetailMessage::IPv4Size;
			if (alreadyTransferred < requestLen)
			{
				// 读取长度不符合
				break;
			}
			auto ipv4 = reinterpret_cast<IPv4*>(addrPtr);
			auto& endpointParser = make_unique<TEndPointParser>(ipv4);
			DoCmd(deadlineSession, endpointParser, requestDetail->Cmd);
		}
		return;
		case ClientRequestDetailMessage::AtypDomainName:
		{
			size_t domainLen = *addrPtr;
			requestLen += domainLen +
				ClientRequestDetailMessage::DomainLenSize +
				ClientRequestDetailMessage::PortSize;

			if (alreadyTransferred < requestLen)
			{
				break;
			}
			char* domainPtr = reinterpret_cast<char*>(addrPtr) + ClientRequestDetailMessage::DomainLenSize;

			auto& endpointParser = make_unique<TEndPointParser>(domainPtr, domainLen);
			DoCmd(deadlineSession, endpointParser, requestDetail->Cmd);
		}
		return;
		case ClientRequestDetailMessage::AtypIPv6:
		{
			requestLen += ClientRequestDetailMessage::IPv6Size;
			if (alreadyTransferred < requestLen)
			{
				break;
			}
			auto ipv6 = reinterpret_cast<IPv6*>(addrPtr);

			auto& endpointParser = make_unique<TEndPointParser>(ipv6);
			DoCmd(deadlineSession, endpointParser, requestDetail->Cmd);
		}
		return;
		default:
			if (alreadyTransferred >= requestLen)
			{
				// 读取长度足够,表示aytp错误
				deadlineSession->Close(ignore);
				return;
			}
			break;
		}
		// 读取长度不足继续读
		timer.expires_from_now(timeoutSecond);

		TcpReadMore(session,
			[this, ptr = _STD move(deadlineSession), alreadyTransferred]
		(const _BOOST system::error_code & error, size_t bytes_transferred) mutable{
			HandleNoAuthenticationRequired(ptr, alreadyTransferred, bytes_transferred, error);
		}, alreadyTransferred);
	}  // HandleNoAuthenticationRequired end

	void Rfc1928::DoCmd(shared_ptr<TTcpDeadlineSession>& deadlineSession, unique_ptr<TEndPointParser>& parser,
		unsigned char cmd)
	{
		GetTrace()->DebugPoint(__func__);
		switch (cmd)
		{
		case ClientRequestDetailMessage::CmdConnect:
			DoCmdConnect(deadlineSession, parser);
			break;
		case ClientRequestDetailMessage::CmdBind:
			// TODO:
			break;
		case ClientRequestDetailMessage::CmdUdpAssociate:
			// TODO:
			break;
		default:
			// X'07' Command not supported
			deadlineSession->Close(ignore);
			break;
		}
	}

	void Rfc1928::DoCmdConnect(shared_ptr<TTcpDeadlineSession>& deadlineSession, unique_ptr<TEndPointParser>& parser)
	{
		GetTrace()->DebugPoint(__func__);
		auto& session = deadlineSession->GetSession();
		switch (parser->GetAtyp())
		{
		case TEndPointParser::AtypIPv4:
		case TEndPointParser::AtypIPv6:
		{

			auto& remote = NewSession<Tcp>();

			auto& remoteRef = *remote;
			remoteRef.AsyncConnect(parser->ParseEndpoint<Tcp>(),
				[this, ptr = _STD move(deadlineSession), remotePtr = _STD move(remote)]
			(const _BOOST system::error_code & error) mutable {
				HandleCmdConnectReply(ptr, remotePtr, error);
			});
		}
		return;
		case TEndPointParser::AtypDomainName:
		{

			GetResolver<Tcp>()->async_resolve(parser->ParseDomain<Tcp>(),
				[this, ptr = _STD move(deadlineSession)]
			(const _BOOST system::error_code & error, const Tcp::resolver::iterator& endpoint_iterator) mutable {
				HandleResolver(ptr, endpoint_iterator, error);
			});
		}
		return;
		default:
			__assume(false);
			break;
		}
	}

	void Rfc1928::HandleResolver(shared_ptr<TTcpDeadlineSession>& deadlineSession,
		const Tcp::resolver::iterator& endpoint_iterator, const _BOOST system::error_code & error)
	{
		GetTrace()->DebugPoint(__func__);
		if (isStopped)
		{
			deadlineSession->Close(ignore);
			return;
		}
		if (error)
		{
			// FIX: 返回解析错误，关闭连接
			deadlineSession->Close(ignore);
			return;
		}

		auto& session = deadlineSession->GetSession();
		auto& remote = NewSession<Tcp>();


		auto& remoteRef = *remote;
		remoteRef.AsyncConnect(endpoint_iterator,
			[this, ptr = _STD move(deadlineSession), remotePtr = _STD move(remote)]
		(const _BOOST system::error_code & error, const Tcp::resolver::iterator& endpoint_iterator) mutable {
			HandleCmdConnectReply(ptr, remotePtr, error);
		});
	}

	// 返回值为整个结构（包括地址）长度
	int Rfc1928::BuildCmdConnectReplyMessage(ServerReplieMessage* reply, shared_ptr<TcpSession>& remote,
		const _BOOST system::error_code & error)
	{
		GetTrace()->DebugPoint(__func__);
		assert(reply->Ver == ServerReplieMessage::VerValue);
		assert(reply->Rsv == ServerReplieMessage::RsvValue);

		int result = sizeof(ServerReplieMessage);
		// 此处error是romote的
		if (!error)
		{
			// 成功
			reply->Rep = ServerReplieMessage::Succeeded;

			auto addrPtr = reinterpret_cast<char*>(reply + 1);
			auto& socket = remote->GetSocket();
			auto& ep = socket.local_endpoint();
			auto& addr = ep.address();

			if (addr.is_v4())
			{
				reply->Atyp = ServerReplieMessage::AtypIPv4;
				auto tmpV4 = reinterpret_cast<IPv4*>(addrPtr);
				auto remoteV4 = reinterpret_cast<unsigned int*>(addr.to_v4().to_bytes().data());
				tmpV4->Ipv4 = _BOOST endian::native_to_big(*remoteV4); //_ASIO detail::socket_ops::host_to_network_long(*remoteV4);
				tmpV4->Port = _BOOST endian::native_to_big(ep.port()); // _ASIO detail::socket_ops::host_to_network_short(ep.port());
				//tmpV4->Ipv4 = *remoteV4;
				//tmpV4->Port = ep.port();
				result += sizeof(IPv4);
			}
			else if (addr.is_v6())
			{
				// FIX: 下面的字节序可能有问题
				reply->Atyp = ServerReplieMessage::AtypIPv6;
				auto tmpV6 = reinterpret_cast<IPv6*>(addrPtr);
				auto& remoteV6 = addr.to_v6().to_bytes();

#ifdef _WINDOWS_
				memcpy_s(tmpV6->Ipv6.data(), tmpV6->Ipv6.size(), remoteV6.data(), remoteV6.size());
#else	
				auto srcBegin = remoteV6.data();
				_STD copy(srcBegin, srcBegin + remoteV6.size(), tmpV6->Ipv6.data());
#endif

				tmpV6->Port = _BOOST endian::native_to_big(ep.port()); //_ASIO detail::socket_ops::host_to_network_short(ep.port());
				result += sizeof(IPv6);
			}
			else
			{
				// 只有2种选择
				__assume(false);
			}

		}
		else if (error == _ASIO error::network_unreachable)
		{
			// 03 network_unreachable
			reply->Rep = ServerReplieMessage::NetworkUnreachable;
		}
		else if (error == _ASIO error::host_unreachable)
		{
			// 04 host_unreachable
			reply->Rep = ServerReplieMessage::HostUnreachable;
		}
		else if (error == _ASIO error::connection_refused)
		{
			// 05 connection_refused
			reply->Rep = ServerReplieMessage::ConnectionRefused;
		}
		else
		{
			// 08 fault _ASIO error::fault
			// 其它没找到 合并过来
			reply->Rep = ServerReplieMessage::GeneralSocksServerFailure;
		}
		return result;
	}

	void Rfc1928::HandleCmdConnectReply(shared_ptr<TTcpDeadlineSession>& deadlineSession,
		shared_ptr<TcpSession>& remote, const _BOOST system::error_code & error)
	{
		GetTrace()->DebugPoint(__func__);
		if (isStopped)
		{
			deadlineSession->Close(ignore);
			remote->Close(ignore);
			return;
		}

		auto& client = deadlineSession->GetSession();
		auto& buffer = client->GetBuffer();
		auto bufferData = buffer.data();
		auto reply = reinterpret_cast<ServerReplieMessage*>(bufferData);
		// 此时client的buffer还保留原来的数值
		auto tmpRequestLen = BuildCmdConnectReplyMessage(reply, remote, error);
		// 如果有错误返回的是除了地址之外的数据然后关闭连接，文档没写失败了地址发什么


		client->AsyncWrite(_ASIO buffer(reply, tmpRequestLen),
			[this, ptr = _STD move(deadlineSession),
			remotePtr = _STD move(remote), isRemoteConnected = !error]
		(const _BOOST system::error_code & writeError, size_t bytes_transferred) mutable
		{
			if (!isRemoteConnected || writeError)
			{
				// client 写失败
				// remote connect 失败
				ptr->Close(ignore);
				remotePtr->Close(ignore);
				return;
			}
			// session脱离deadline
			ptr->CancelTimer(ignore);
			// 创建转发关系
			CreateForward(ptr->GetSession(), remotePtr);
		});


	}

	void Rfc1928::CreateForward(shared_ptr<TcpSession>& client, shared_ptr<TcpSession>& remote)
	{
		GetTrace()->DebugPoint(__func__);
		client->AsyncReadSome(_ASIO buffer(client->GetBuffer()),
			[this, ptr = make_shared<TTcpForward>(client, remote)]
		(const _BOOST system::error_code & error, size_t bytes_transferred) mutable {
			ForwardRead(ptr, error, bytes_transferred);
		});
		remote->AsyncReadSome(_ASIO buffer(remote->GetBuffer()),
			[this, ptr = make_shared<TTcpForward>(remote, client)]
		(const _BOOST system::error_code & error, size_t bytes_transferred) mutable {
			ForwardRead(ptr, error, bytes_transferred);
		});
	}

	void Rfc1928::ForwardRead(shared_ptr<TTcpForward>& forward, const _BOOST system::error_code & error,
		size_t bytes_transferred)
	{
		auto& writeSession = forward->GetWriteSession();
		if (error || isStopped)
		{
			if (writeSession->IsOpen())
			{
				if (error == _ASIO error::eof)
				{
					writeSession->ShutDown(_ASIO socket_base::shutdown_both, ignore);
				}
				else
				{
					writeSession->Close(ignore);
				}
				//
			}
			return;
		}
		auto& readSession = forward->GetReadSession();
		writeSession->AsyncWrite(_ASIO buffer(readSession->GetBuffer(), bytes_transferred),
			[this, ptr = _STD move(forward)](const _BOOST system::error_code & error, size_t bytes_transferred) mutable {
			ForwardWrite(ptr, error, bytes_transferred);
		});
	}

	void Rfc1928::ForwardWrite(shared_ptr<TTcpForward>& forward, const _BOOST system::error_code & error,
		size_t bytes_transferred)
	{
		auto& readSession = forward->GetReadSession();
		if (error || isStopped)
		{
			if (readSession->IsOpen())
			{
				if (error == _ASIO error::eof)
				{
					readSession->ShutDown(_ASIO socket_base::shutdown_both, ignore);
				}
				else
				{
					readSession->Close(ignore);
				}
				//
			}
			return;
		}
		auto& readBuffer = forward->GetReadBuffer();
		readSession->AsyncReadSome(readBuffer,
			[this, ptr = _STD move(forward)](const _BOOST system::error_code & error, size_t bytes_transferred) mutable {
			ForwardRead(ptr, error, bytes_transferred);
		});
	}
}  // namespace FeedTheDog


