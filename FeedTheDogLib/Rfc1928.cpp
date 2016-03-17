#include "stdafx.h"
#include "Rfc1928.h"
namespace FeedTheDog
{
	Rfc1928::Rfc1928(int port, const char* name, int timeout, int closeDelay)
		:ServiceBase(name),
		port_(port),
		timeoutSecond(timeout),
		closeSocketDelaySecond(closeDelay)
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

		auto& session = core->SelectIdleWorker()->NewSession<_ASIO ip::tcp>(name_);
		// buffer����1k
		assert(session->GetBuffer().max_size() > 1024);

		session->GetTimer().async_wait(_BOOST bind(&Rfc1928::CheckDeadline, this, session, _ASIO placeholders::error));

		acceptor->async_accept(session->GetSocket(),
			_BOOST bind(&Rfc1928::HandleAccept, this, session, _ASIO placeholders::error));

	}
	void Rfc1928::CheckDeadline(shared_ptr<TTcpSession>& session, const _BOOST system::error_code& error)
	{
		if (error || isStopped)
		{
			return;
		}
		auto& timer = session->GetTimer();
		if (timer.expires_at() <= _ASIO deadline_timer::traits_type::now())
		{
			session->Close();
			return;
		}
		timer.async_wait(_BOOST bind(&Rfc1928::CheckDeadline, this, session, _ASIO placeholders::error));

	}
	void Rfc1928::Stop()
	{
		isStopped = true;
		acceptor->close();
	}

	void Rfc1928::HandleAccept(shared_ptr<TTcpSession>& session, const _BOOST system::error_code & error)
	{
		if (error || isStopped)
		{
			return;
		}
		session->GetTimer().expires_from_now(timeoutSecond);
		Async::TcpReadMore(session, _BOOST bind(&Rfc1928::HandleReadVersionMessage, this, session,
			0, _ASIO placeholders::bytes_transferred, _ASIO placeholders::error));
		AsyncStart();
	}

	bool Rfc1928::CheckVersionMessage(size_t alreadyTransferred, const VersionMessage* vmHeader)
	{
		return alreadyTransferred >= ((size_t)vmHeader->NMethods + VersionMessage::FirstTwoBytes);
	}
	// ����ѡ��ĺ���
	unsigned char Rfc1928::ReplySelectedMethod(_ASIO ip::tcp::socket& sessionSocket, VersionMessage* vmHeader, _BOOST system::error_code & error)
	{
		unsigned char nmethods = vmHeader->NMethods;
		// ʹ��ԭ����buffer�洢������Ϣ
		auto replyBuffer = reinterpret_cast<MethodSelectionMessage*>(vmHeader);
		replyBuffer->Method = MethodSelectionMessage::NoAcceptableMethods;
		// ѡ����
		for (size_t i = 0; i < nmethods; i++)
		{
			auto& tmpMethod = vmHeader->Methods[i];
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
	void Rfc1928::RunMethod(shared_ptr<TTcpSession>& session, unsigned char selectedMethod)
	{
		auto& timer = session->GetTimer();
		switch (selectedMethod)
		{
		case NoAuthenticationRequired:
			timer.expires_from_now(timeoutSecond);
			Async::TcpReadMore(session, _BOOST bind(&Rfc1928::HandleNoAuthenticationRequired, this, session, 0,
				_ASIO placeholders::bytes_transferred, _ASIO placeholders::error));
			break;
		case Gssapi:
			// TODO:
			break;
		case UsernamePassword:
			// TODO:
			break;
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
				session->GetTimer().expires_from_now(closeSocketDelaySecond);
			}
			break;
		}
	}
	void Rfc1928::HandleReadVersionMessage(shared_ptr<TTcpSession>& session, size_t alreadyTransferred,
		size_t bytes_transferred, const _BOOST system::error_code & error)
	{
		if (error || isStopped)
		{
			return;
		}
		auto& timer = session->GetTimer();
		timer.expires_at(_BOOST posix_time::pos_infin);
		alreadyTransferred += bytes_transferred;
		auto& sessionSocket = session->GetSocket();
		auto& buffer = session->GetBuffer();
		auto bufferData = buffer.data();

		auto vmHeader = reinterpret_cast<VersionMessage*>(bufferData);
		if (vmHeader->Ver != VersionMessage::VerValue)
		{
			session->Close();
			return;
		}
		if (!CheckVersionMessage(alreadyTransferred, vmHeader))
		{
			timer.expires_from_now(timeoutSecond);
			Async::TcpReadMore(session, _BOOST bind(&Rfc1928::HandleReadVersionMessage, this, session, alreadyTransferred,
				_ASIO placeholders::bytes_transferred, _ASIO placeholders::error), alreadyTransferred);
			return;
		}
		// ��ȡ������ȷ���ȣ�Э�̹���δ�������ͻ��˶෢������
		_BOOST system::error_code replyError;
		auto selectedMethod = ReplySelectedMethod(sessionSocket, vmHeader, replyError);
		if (!replyError)
		{
			RunMethod(session, selectedMethod);
		}
	}

	// FIX: ��������
	void Rfc1928::HandleNoAuthenticationRequired(shared_ptr<TTcpSession>& session, size_t alreadyTransferred,
		size_t bytes_transferred, const _BOOST system::error_code & error)
	{
		if (isStopped || error)
		{
			return;
		}
		auto& timer = session->GetTimer();
		timer.expires_at(_BOOST posix_time::pos_infin);
		alreadyTransferred += bytes_transferred;
		auto& sessionSocket = session->GetSocket();
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
			session->Close();
			return;
		}

		shared_ptr<EndPointParser> endpointParser;
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
			endpointParser = make_shared<EndPointParser>(ipv4);
			DoCmd(session, endpointParser, requestDetail->Cmd);
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

			endpointParser = make_shared<EndPointParser>(domainPtr, domainLen);
			DoCmd(session, endpointParser, requestDetail->Cmd);
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

			endpointParser = make_shared<EndPointParser>(ipv6);
			DoCmd(session, endpointParser, requestDetail->Cmd);
		}
		return;
		default:
			if (alreadyTransferred >= requestLen)
			{
				// ��ȡ�����㹻,��ʾaytp����
				session->Close();
				return;
			}
			break;
		}
		// ��ȡ���Ȳ��������
		timer.expires_from_now(timeoutSecond);
		Async::TcpReadMore(session, _BOOST bind(&Rfc1928::HandleNoAuthenticationRequired, this, session, alreadyTransferred,
			_ASIO placeholders::bytes_transferred, _ASIO placeholders::error), alreadyTransferred);
	}
	void Rfc1928::DoCmd(shared_ptr<TTcpSession>& session, shared_ptr<EndPointParser>& parser, unsigned char cmd)
	{
		switch (cmd)
		{
		case ClientRequestDetailMessage::CmdConnect:
		{
			DoCmdConnect(session, parser);
		}
		break;
		case ClientRequestDetailMessage::CmdBind:
			break;
		case ClientRequestDetailMessage::CmdUdpAssociate:
			break;
		default:
			// X'07' Command not supported
			session->Close();
			break;
		}
	}
	void Rfc1928::DoCmdConnect(shared_ptr<TTcpSession>& session, shared_ptr<EndPointParser>& parser)
	{
		switch (parser->GetAtyp())
		{
		case EndPointParser::AtypIPv4:
		case EndPointParser::AtypIPv6:
		{
			// ���봴����session����ͬһ��ioservice������
			auto& remote = session->GetWorker()->NewSession<_ASIO ip::tcp>(name_);

			remote->GetSocket().async_connect(parser->ParseEndpoint<_ASIO ip::tcp>(),
				_BOOST bind(&Rfc1928::HandleCmdConnectReply, this, session, remote, _ASIO placeholders::error));
		}
		return;
		case EndPointParser::AtypDomainName:
		{
			session->GetWorker()->GetResolver<_ASIO ip::tcp>().async_resolve(
				parser->ParseDomain<_ASIO ip::tcp>(),
				_BOOST bind(&Rfc1928::HandleResolver, this, session, _ASIO placeholders::iterator, _ASIO placeholders::error));
		}
		return;
		default:
			__assume(false);
			break;
		}
	}


	void Rfc1928::HandleResolver(shared_ptr<TTcpSession>& session, const _ASIO ip::tcp::resolver::iterator& endpoint_iterator,
		const _BOOST system::error_code & error)
	{
		if (isStopped)
		{
			return;
		}
		if (error)
		{
			// FIX: ���ؽ������󣬹ر�����
			session->Close();
			return;
		}

		auto& remote = session->GetWorker()->NewSession<_ASIO ip::tcp>(name_);

		_ASIO async_connect(remote->GetSocket(), endpoint_iterator, _BOOST bind(&Rfc1928::HandleCmdConnectReply,
			this, session, remote, _ASIO placeholders::error));
	}

	// ����ֵΪ�����ṹ��������ַ������
	int Rfc1928::BuildCmdConnectReplyMessage(ServerReplieMessage* reply, shared_ptr<TTcpSession>& remote, const _BOOST system::error_code & error)
	{

		assert(reply->Ver == ServerReplieMessage::VerValue);
		assert(reply->Rsv == ServerReplieMessage::RsvValue);

		int result = sizeof(ServerReplieMessage);
		// �˴�error��romote��
		if (!error)
		{
			// �ɹ�
			reply->Rep = ServerReplieMessage::Succeeded;

			auto addrPtr = reinterpret_cast<char*>(reply + 1);
			auto& socket = remote->GetSocket();
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
	void Rfc1928::HandleCmdConnectReply(shared_ptr<TTcpSession>& client, shared_ptr<TTcpSession>& remote, const _BOOST system::error_code & error)
	{
		if (isStopped)
		{
			return;
		}
		auto& buffer = client->GetBuffer();
		auto bufferData = buffer.data();
		auto reply = reinterpret_cast<ServerReplieMessage*>(bufferData);
		// ��ʱclient��buffer������ԭ������ֵ
		auto tmpRequestLen = BuildCmdConnectReplyMessage(reply, remote, error);
		// ����д��󷵻ص��ǳ��˵�ַ֮�������Ȼ��ر����ӣ��ĵ�ûдʧ���˵�ַ��ʲô
		_BOOST system::error_code writeError;
		_ASIO write(client->GetSocket(), _ASIO buffer(reply, tmpRequestLen), writeError);
		if (error)
		{
			client->GetTimer().expires_from_now(closeSocketDelaySecond);
			return;
		}
		if (writeError)
		{
			return;
		}
		// ����ת����ϵ
		CreateForward(client, remote);

	}
	void Rfc1928::CreateForward(shared_ptr<TTcpSession>& client, shared_ptr<TTcpSession>& remote)
	{
		client->GetSocket().async_read_some(_ASIO buffer(client->GetBuffer()), _BOOST bind(&Rfc1928::HandleForwardRead, this,
			client.get(), remote, _ASIO placeholders::bytes_transferred, _ASIO placeholders::error));
		remote->GetSocket().async_read_some(_ASIO buffer(remote->GetBuffer()), _BOOST bind(&Rfc1928::HandleForwardRead, this,
			remote.get(), client, _ASIO placeholders::bytes_transferred, _ASIO placeholders::error));
	}

	// FIX: ����ָ���bind�ܺ�����
	void Rfc1928::HandleForwardRead(TTcpSession* read, shared_ptr<TTcpSession>& write, size_t bytes_transferred,
		const _BOOST system::error_code & error)
	{
		if (error || isStopped)
		{
			write->Close();
			return;
		}

		write->GetSocket().async_write_some(_ASIO buffer(read->GetBuffer(), bytes_transferred),
			_BOOST bind(&Rfc1928::HandleForwardWrite, this, read, write, _ASIO placeholders::bytes_transferred, _ASIO placeholders::error));
	}
	void Rfc1928::HandleForwardWrite(TTcpSession* read, shared_ptr<TTcpSession>& write, size_t bytes_transferred,
		const _BOOST system::error_code & error)
	{
		if (error || isStopped)
		{
			read->Close();
			return;
		}
		read->GetSocket().async_read_some(_ASIO buffer(read->GetBuffer()), _BOOST bind(&Rfc1928::HandleForwardRead, this,
			read, write, _ASIO placeholders::bytes_transferred, _ASIO placeholders::error));
	}
}  // namespace FeedTheDog


