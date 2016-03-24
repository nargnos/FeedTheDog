#include "stdafx.h"
#include "Rfc1928.h"
namespace FeedTheDog
{
	Rfc1928::Rfc1928(int port, const char* name, int timeout)
		:ServiceBase(name),
		port_(port),
		timeoutSecond(timeout)
	{
		isStopped = false;
	}


	Rfc1928::~Rfc1928()
	{
	}
	bool Rfc1928::Init(TCore * corePtr)
	{
		isStopped = false;
		core = corePtr;
		if (!acceptor)
		{
			auto& io = corePtr->SelectIdleWorker()->GetIoService();
			// ipv4�����ٸ�
			acceptor = make_unique<_ASIO ip::tcp::acceptor>(io, _ASIO ip::tcp::endpoint(_ASIO ip::tcp::v4(), port_));
			// ����֧�ֵĺ���
			memset(supportMethods, false, 0xff);
			// FIX: ��Ҫ�����ö�ȡ���ò��Դ���
			supportMethods[Rfc1928::NoAuthenticationRequired] = true;
			// gssapi
			// username & password
			// 0x03 - 0x7f iana
			// 0x80 - 0xfe reserved
		}
		return true;
	}
	void Rfc1928::AsyncStart()
	{
		if (isStopped)
		{
			return;
		}
		core->GetTrace()->DebugPoint(__func__);
		auto& session = core->SelectIdleWorker()->NewSession<_ASIO ip::tcp>();
		// buffer�������1k
		assert(session->GetBuffer().max_size() > 1024);
		auto& deadlineSession = make_shared<TTcpDeadlineSession>(_STD move(session));
		deadlineSession->GetTimer().async_wait(_BOOST bind(&Rfc1928::CheckDeadline, this, deadlineSession, _ASIO placeholders::error));

		acceptor->async_accept(*deadlineSession->GetSession(),
			_BOOST bind(&Rfc1928::HandleAccept, this, deadlineSession, _ASIO placeholders::error));

	}

	void Rfc1928::CheckDeadline(shared_ptr<TTcpDeadlineSession>& deadlineSession, const _BOOST system::error_code& error)
	{
		core->GetTrace()->DebugPoint(__func__);
		if (error || isStopped)
		{
			return;
		}
		auto& timer = deadlineSession->GetTimer();
		if (timer.expires_at() <= _ASIO deadline_timer::traits_type::now())
		{
			deadlineSession->GetSession()->close(ignore);
			return;
		}
		timer.async_wait(_BOOST bind(&Rfc1928::CheckDeadline, this, deadlineSession, _ASIO placeholders::error));


	}
	void Rfc1928::Stop()
	{
		core->GetTrace()->DebugPoint("stop",false,0,name_);
		isStopped = true;
		acceptor->close();
	}

	void Rfc1928::HandleAccept(shared_ptr<TTcpDeadlineSession>& deadlineSession, const _BOOST system::error_code & error)
	{
		core->GetTrace()->DebugPoint(__func__);
		if (error || isStopped)
		{
			deadlineSession->Close(ignore);
			return;
		}
		deadlineSession->GetTimer().expires_from_now(timeoutSecond);
		auto& session = deadlineSession->GetSession();
		session->async_read_some(_ASIO buffer(session->GetBuffer()), _BOOST bind(&Rfc1928::HandleReadVersionMessage, this, deadlineSession,
			0, _ASIO placeholders::bytes_transferred, _ASIO placeholders::error));
		AsyncStart();
	}
	void Rfc1928::HandleReadVersionMessage(shared_ptr<TTcpDeadlineSession>& deadlineSession, size_t alreadyTransferred,
		size_t bytes_transferred, const _BOOST system::error_code & error)
	{
		core->GetTrace()->DebugPoint(__func__);
		if (error || isStopped)
		{
			deadlineSession->Close(ignore);
			return;
		}
		auto& session = deadlineSession->GetSession();
		auto& timer = deadlineSession->GetTimer();
		timer.expires_at(_BOOST posix_time::pos_infin);
		alreadyTransferred += bytes_transferred;

		auto& sessionSocket = *session;
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
			TUtil::TcpReadMore(session, _BOOST bind(&Rfc1928::HandleReadVersionMessage, this, deadlineSession, alreadyTransferred,
				_ASIO placeholders::bytes_transferred, _ASIO placeholders::error), alreadyTransferred);
			return;
		}
		// ��ȡ������ȷ���ȣ�Э�̹���δ�������ͻ��˶෢������
		_BOOST system::error_code replyError;
		auto selectedMethod = ReplySelectedMethod(sessionSocket, vmHeader, replyError);
		if (!replyError)
		{
			RunMethod(deadlineSession, selectedMethod);
		}
	}

	bool Rfc1928::CheckVersionMessage(size_t alreadyTransferred, const VersionMessage* vmHeader)
	{
		return alreadyTransferred >= ((size_t)vmHeader->NMethods + VersionMessage::FirstTwoBytes);
	}
	// ����ѡ��ĺ���
	unsigned char Rfc1928::ReplySelectedMethod(_ASIO ip::tcp::socket& sessionSocket, VersionMessage* vmHeader, _BOOST system::error_code & error)
	{
		core->GetTrace()->DebugPoint(__func__);
		unsigned char nmethods = vmHeader->NMethods;
		// ʹ��ԭ����buffer�洢������Ϣ
		auto replyBuffer = reinterpret_cast<MethodSelectionMessage*>(vmHeader);
		replyBuffer->Method = MethodSelectionMessage::NoAcceptableMethods;
		// ѡ����
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
		// ���ظ��ͻ���
		_ASIO write(sessionSocket, _ASIO buffer(replyBuffer, sizeof(MethodSelectionMessage)), error);
		return replyBuffer->Method;
	}
	void Rfc1928::RunMethod(shared_ptr<TTcpDeadlineSession>& deadlineSession, unsigned char selectedMethod)
	{
		core->GetTrace()->DebugPoint(__func__);
		auto& timer = deadlineSession->GetTimer();
		auto& session = deadlineSession->GetSession();
		switch (selectedMethod)
		{
		case NoAuthenticationRequired:
			timer.expires_from_now(timeoutSecond);
			TUtil::TcpReadMore(session, _BOOST bind(&Rfc1928::HandleNoAuthenticationRequired, this, deadlineSession, 0,
				_ASIO placeholders::bytes_transferred, _ASIO placeholders::error));
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

	// FIX: ��������
	void Rfc1928::HandleNoAuthenticationRequired(shared_ptr<TTcpDeadlineSession>& deadlineSession, size_t alreadyTransferred,
		size_t bytes_transferred, const _BOOST system::error_code & error)
	{
		core->GetTrace()->DebugPoint(__func__);
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
		// ��ַ���ݿ�ʼλ��
		size_t requestLen = sizeof(ClientRequestDetailMessage);
		auto addrPtr = bufferData + requestLen;

		// �����ȡ�㹻���ȣ�Ԥ���жϱ�ʶ
		if (requestDetail->Ver != ClientRequestDetailMessage::VerValue ||
			requestDetail->Rsv != ClientRequestDetailMessage::RsvValue)
		{
			deadlineSession->Close(ignore);
			return;
		}

		
		// �����ȡ�㹻���ȣ�Ԥ�ȴ���aytp
		switch (requestDetail->Atyp)
		{
		case ClientRequestDetailMessage::AtypIPv4:
		{
			requestLen += ClientRequestDetailMessage::IPv4Size;
			if (alreadyTransferred < requestLen)
			{
				// ��ȡ���Ȳ�����
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
			requestLen += domainLen + ClientRequestDetailMessage::DomainLenSize + ClientRequestDetailMessage::PortSize;
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
				// ��ȡ�����㹻,��ʾaytp����
				deadlineSession->Close(ignore);
				return;
			}
			break;
		}
		// ��ȡ���Ȳ��������
		timer.expires_from_now(timeoutSecond);
		TUtil::TcpReadMore(session, _BOOST bind(&Rfc1928::HandleNoAuthenticationRequired, this, deadlineSession, alreadyTransferred,
			_ASIO placeholders::bytes_transferred, _ASIO placeholders::error), alreadyTransferred);
	}
	void Rfc1928::DoCmd(shared_ptr<TTcpDeadlineSession>& deadlineSession, unique_ptr<TEndPointParser>& parser, unsigned char cmd)
	{
		core->GetTrace()->DebugPoint(__func__);
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
		core->GetTrace()->DebugPoint(__func__);
		auto& session = deadlineSession->GetSession();
		switch (parser->GetAtyp())
		{
		case TEndPointParser::AtypIPv4:
		case TEndPointParser::AtypIPv6:
		{
			// ���봴����session����ͬһ��ioservice������
			auto& remote = session->GetWorker()->NewSession<_ASIO ip::tcp>();

			remote->async_connect(parser->ParseEndpoint<_ASIO ip::tcp>(),
				_BOOST bind(&Rfc1928::HandleCmdConnectReply, this, deadlineSession, remote, _ASIO placeholders::error));
		}
		return;
		case TEndPointParser::AtypDomainName:
		{
			session->GetWorker()->GetResolver<_ASIO ip::tcp>().async_resolve(
				parser->ParseDomain<_ASIO ip::tcp>(),
				_BOOST bind(&Rfc1928::HandleResolver, this, deadlineSession, _ASIO placeholders::iterator, _ASIO placeholders::error));
		}
		return;
		default:
			__assume(false);
			break;
		}
	}


	void Rfc1928::HandleResolver(shared_ptr<TTcpDeadlineSession>& deadlineSession, const _ASIO ip::tcp::resolver::iterator& endpoint_iterator,
		const _BOOST system::error_code & error)
	{
		core->GetTrace()->DebugPoint(__func__);
		if (isStopped)
		{
			deadlineSession->Close(ignore);
			return;
		}
		if (error)
		{
			// FIX: ���ؽ������󣬹ر�����
			deadlineSession->Close(ignore);
			return;
		}

		auto& session = deadlineSession->GetSession();
		auto& remote = session->GetWorker()->NewSession<_ASIO ip::tcp>();

		_ASIO async_connect(*remote, endpoint_iterator, _BOOST bind(&Rfc1928::HandleCmdConnectReply,
			this, deadlineSession, remote, _ASIO placeholders::error));
	}

	// ����ֵΪ�����ṹ��������ַ������
	int Rfc1928::BuildCmdConnectReplyMessage(ServerReplieMessage* reply, shared_ptr<TTcpSession>& remote, const _BOOST system::error_code & error)
	{
		core->GetTrace()->DebugPoint(__func__);
		assert(reply->Ver == ServerReplieMessage::VerValue);
		assert(reply->Rsv == ServerReplieMessage::RsvValue);

		int result = sizeof(ServerReplieMessage);
		// �˴�error��romote��
		if (!error)
		{
			// �ɹ�
			reply->Rep = ServerReplieMessage::Succeeded;

			auto addrPtr = reinterpret_cast<char*>(reply + 1);
			auto& socket = *remote;
			auto& ep = socket.local_endpoint();
			auto& addr = ep.address();

			if (addr.is_v4())
			{
				reply->Atyp = ServerReplieMessage::AtypIPv4;
				auto tmpV4 = reinterpret_cast<IPv4*>(addrPtr);
				auto remoteV4 = reinterpret_cast<unsigned long*>(addr.to_v4().to_bytes().data());
				tmpV4->Ipv4 = _ASIO detail::socket_ops::host_to_network_long(*remoteV4);
				tmpV4->Port = _ASIO detail::socket_ops::host_to_network_short(ep.port());
				//tmpV4->Ipv4 = *remoteV4;
				//tmpV4->Port = ep.port();
				result += sizeof(IPv4);
			}
			else if (addr.is_v6())
			{
				// FIX: ������ֽ������������
				reply->Atyp = ServerReplieMessage::AtypIPv6;
				auto tmpV6 = reinterpret_cast<IPv6*>(addrPtr);
				auto& remoteV6 = addr.to_v6().to_bytes();
				memcpy_s(tmpV6->Ipv6.data(), tmpV6->Ipv6.size(), remoteV6.data(), remoteV6.size());
				tmpV6->Port = _ASIO detail::socket_ops::host_to_network_short(ep.port());
				result += sizeof(IPv6);
			}
			else
			{
				// ֻ��2��ѡ��
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
			// ����û�ҵ� �ϲ�����
			reply->Rep = ServerReplieMessage::GeneralSocksServerFailure;
		}
		return result;
	}
	void Rfc1928::HandleCmdConnectReply(shared_ptr<TTcpDeadlineSession>& deadlineSession, shared_ptr<TTcpSession>& remote, const _BOOST system::error_code & error)
	{
		core->GetTrace()->DebugPoint(__func__);
		if (isStopped)
		{
			deadlineSession->Close(ignore);
			remote->close(ignore);
			return;
		}

		auto& client = deadlineSession->GetSession();
		auto& buffer = client->GetBuffer();
		auto bufferData = buffer.data();
		auto reply = reinterpret_cast<ServerReplieMessage*>(bufferData);
		// ��ʱclient��buffer������ԭ������ֵ
		auto tmpRequestLen = BuildCmdConnectReplyMessage(reply, remote, error);
		// ����д��󷵻ص��ǳ��˵�ַ֮�������Ȼ��ر����ӣ��ĵ�ûдʧ���˵�ַ��ʲô
		_BOOST system::error_code writeError;
		_ASIO write(*client, _ASIO buffer(reply, tmpRequestLen), writeError);
		//	auto& timer = deadlineSession->GetTimer();
		if (error || writeError)
		{
			// client дʧ��
			// remote connect ʧ��
			//timer.expires_from_now(closeSocketDelaySecond);
			deadlineSession->Close(ignore);
			remote->close(ignore);
			return;
		}
		// session����deadline
		deadlineSession->CancelTimer(ignore);
		// ����ת����ϵ
		CreateForward(client, remote);

	}
	void Rfc1928::CreateForward(shared_ptr<TTcpSession>& client, shared_ptr<TTcpSession>& remote)
	{
		core->GetTrace()->DebugPoint(__func__);

		TTcpForward clientForward(client, remote, this);
		TTcpForward remoteForward(remote, client, this);

		client->async_read_some(clientForward.GetReadBuffer(), _STD move(clientForward));
		remote->async_read_some(remoteForward.GetReadBuffer(), _STD move(remoteForward));
	}

}  // namespace FeedTheDog


