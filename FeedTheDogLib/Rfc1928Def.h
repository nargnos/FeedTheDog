#pragma once
namespace FeedTheDog
{
	// 客户机要用加密方式连接并提交请求，服务器得到数据后再加密返回客户端
	// 客户机要在本地有一个解密的服务端（服务器和客户机的都需要监听），然后用支持1928的程序连接就可以了
	enum class Rfc1928ServiceType
	{
		Remote,
		Local
		// 其它扩展待编写
	};
	// 根据协议原文声明类型
#pragma pack(push,1)
#pragma warning(disable : 4200)
	// client连接server发送 版本标识和认证函数选择 信息
	// +----+----------+----------+
	// |VER | NMETHODS | METHODS  |
	// +----+----------+----------+
	// | 1  |    1     | 1 to 255 |
	// +----+----------+----------+
	struct VersionMessage
	{
		enum : unsigned short
		{
			FirstTwoBytes = 2,
			VerValue = 0x05,
			MaxSize = 255 + FirstTwoBytes
		};
		unsigned char Ver;
		unsigned char NMethods;
		unsigned char Methods[];
	};
	// server选择一个支持的函数返回

	// +----+--------+
	// |VER | METHOD |
	// +----+--------+
	// | 1  |   1    |
	// +----+--------+
	// If the selected METHOD is X'FF', none of the methods listed by the
	// client are acceptable, and the client MUST close the connection.
	//   X'00' NO AUTHENTICATION REQUIRED
	//   X'01' GSSAPI（通用安全服务应用程序接口），Compliant implementations MUST support GSSAPI and SHOULD support USERNAME / PASSWORD authentication methods.
	//   X'02' USERNAME / PASSWORD
	//   X'03' to X'7F' IANA ASSIGNED
	//   X'80' to X'FE' 私有函数保留位
	//   X'FF' 不接受客户端请求的函数
	struct MethodSelectionMessage
	{
		enum : unsigned char
		{
			VerValue = VersionMessage::VerValue,
			NoAcceptableMethods = 0xFF,
		};
		unsigned char Ver;
		// X'FF'关连接
		// x01先放着吧
		unsigned char Method;
	};

	// 前面沟通完毕后client发送请求详细信息
	//    +----+-----+-------+------+----------+----------+
	//    |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
	//    +----+-----+-------+------+----------+----------+
	//    | 1  |  1  | X'00' |  1   | Variable |    2     |
	//    +----+-----+-------+------+----------+----------+
	//
	// Where:
	//
	//      o  VER    protocol version: X'05'
	//      o  CMD
	//         o  CONNECT X'01'
	//         o  BIND X'02'
	//         o  UDP ASSOCIATE X'03'
	//      o  RSV    RESERVED
	//      o  ATYP   address type of following address
	//         o  IP V4 address: X'01'
	//         o  DOMAINNAME: X'03'
	//         o  IP V6 address: X'04'
	//      o  DST.ADDR       desired destination address
	//      o  DST.PORT desired destination port in network octet
	//         order
	struct IPv4
	{
		unsigned long Ipv4;
		unsigned short Port;
	};
	struct DomainName
	{
		unsigned char Len;
		unsigned char Name[];
		// unsigned short Port;
	};
	struct IPv6
	{
		enum
		{
			AddressSize = 16
		};
		_STD array<unsigned char, AddressSize> Ipv6;

		unsigned short Port;
	};
	struct ClientRequestDetailMessage
	{
		enum : unsigned char
		{
			RsvValue = 0x00,
			VerValue = VersionMessage::VerValue,
			IPv4Size = sizeof(IPv4),
			IPv6Size = sizeof(IPv6),
			DomainLenSize = 0x01,
			PortSize = 2,
		};
		enum Atyp :unsigned char
		{
			AtypIPv4 = 0x01,
			AtypDomainName = 0x03,
			AtypIPv6 = 0x04,
		};
		enum Cmd :unsigned char
		{
			CmdConnect = 0x01,
			CmdBind = 0x02,
			CmdUdpAssociate = 0x03,
		};
		unsigned char Ver;
		unsigned char Cmd;
		unsigned char Rsv;
		unsigned char Atyp;
		// Dst.Addr v Dst.Port 2
	};

	// 服务器返回
	// +----+-----+-------+------+----------+----------+
	// |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
	// +----+-----+-------+------+----------+----------+
	// | 1  |  1  | X'00' |  1   | Variable |    2     |
	// +----+-----+-------+------+----------+----------+
	// 
	// Where:
	// 
	//   o  VER    protocol version: X'05'
	//   o  REP    Reply field:
	//      o  X'00' succeeded
	//      o  X'01' general SOCKS server failure
	//      o  X'02' connection not allowed by ruleset
	//      o  X'03' Network unreachable
	//      o  X'04' Host unreachable
	//      o  X'05' Connection refused
	//      o  X'06' TTL expired
	//      o  X'07' Command not supported
	//      o  X'08' Address type not supported
	//      o  X'09' to X'FF' unassigned
	//   o  RSV    RESERVED
	//   o  ATYP   address type of following address
	//     o  IP V4 address: X'01'
	//     o  DOMAINNAME: X'03'
	//     o  IP V6 address: X'04'
	//   o  BND.ADDR       server bound address
	//   o  BND.PORT       server bound port in network octet order
	struct ServerReplieMessage
	{
		enum
		{
			RsvValue = 0x00,
			VerValue = VersionMessage::VerValue,
		};
		enum RepValue
		{
			Succeeded = 0x00,
			GeneralSocksServerFailure = 0x01,
			ConnectionNotAllowedByRuleset = 0x02,
			NetworkUnreachable = 0x03,
			HostUnreachable = 0x04,
			ConnectionRefused = 0x05,
			TtlExpired = 0x06,
			CommandNotSupported = 0x07,
			AddressTypeNotSupported = 0x08,
		};
		enum Atyp :unsigned char
		{
			AtypIPv4 = ClientRequestDetailMessage::AtypIPv4,
			AtypDomainName = ClientRequestDetailMessage::AtypDomainName,
			AtypIPv6 = ClientRequestDetailMessage::AtypIPv6,
		};
		unsigned char Ver;
		unsigned char Rep;
		unsigned char Rsv;
		unsigned char Atyp;
		// Bnd.Addr v Bnd.Port 2
	};
	// Udp用的
	// +----+------+------+----------+----------+----------+
	// |RSV | FRAG | ATYP | DST.ADDR | DST.PORT |   DATA   |
	// +----+------+------+----------+----------+----------+
	// | 2  |  1   |  1   | Variable |    2     | Variable |
	// +----+------+------+----------+----------+----------+
	// 
	// The fields in the UDP request header are:
	// 
	//     o  RSV  Reserved X'0000'
	//     o  FRAG    Current fragment number
	//     o  ATYP    address type of following addresses:
	//        o  IP V4 address: X'01'
	//        o  DOMAINNAME: X'03'
	//        o  IP V6 address: X'04'
	//     o  DST.ADDR       desired destination address
	//     o  DST.PORT       desired destination port
	//     o  DATA     user data

	struct UdpRrequestHeader
	{
		unsigned short Rsv;
		unsigned char Frag;
		unsigned char Aytp;
		// Dst.Addr v \ Dst.Port 2\ Data v
	};
#pragma pack(pop)


	// 除了domain字符串，其传入结构都是网络序
	class EndPointParser:
		public _BOOST noncopyable
	{
	public:
		enum Atyp :unsigned char
		{
			AtypIPv4 = ClientRequestDetailMessage::AtypIPv4,
			AtypDomainName = ClientRequestDetailMessage::AtypDomainName,
			AtypIPv6 = ClientRequestDetailMessage::AtypIPv6,
		};
		EndPointParser(const IPv4* val) :
			aytp(AtypIPv4),
			port(_BOOST endian::big_to_native(val->Port))
		{
			// _ASIO detail::socket_ops::network_to_host_short(parser->Port)

			new (GetBufferPtr()) _ASIO ip::address_v4(_ASIO detail::socket_ops::network_to_host_long(val->Ipv4));
		}
		// FIX: ipv6字节序
		EndPointParser(const IPv6* val) :
			aytp(AtypIPv6),
			port(_BOOST endian::big_to_native(val->Port))
		{
			//_ASIO detail::socket_ops::network_to_host_short(parser->Port)
			new (GetBufferPtr()) _ASIO ip::address_v6(val->Ipv6);
		}
		// domainPtr字符串（无NULL结尾）后必须跟2位端口号
		EndPointParser(const char* domainPtr, size_t domainLen) :
			aytp(AtypDomainName),
			port(_BOOST endian::big_to_native(*reinterpret_cast<const unsigned short*>(domainPtr + domainLen)))
		{
			// _ASIO detail::socket_ops::network_to_host_short(*reinterpret_cast<const unsigned short*>(domainPtr + domainLen))
			new (GetBufferPtr()) _STD string(domainPtr, domainLen);
		}
	
		~EndPointParser()
		{
			switch (aytp)
			{
			case FeedTheDog::EndPointParser::AtypIPv4:
				GetV4Addr()->~address_v4();
				break;
			case FeedTheDog::EndPointParser::AtypDomainName:
				GetDomain()->~basic_string();
				break;
			case FeedTheDog::EndPointParser::AtypIPv6:
				GetV6Addr()->~address_v6();
				break;
			default:
				break;
			}
		}
		template<typename TProtocol>
		typename TProtocol::endpoint ParseEndpoint()
		{
			switch (aytp)
			{
			case EndPointParser::AtypIPv4:
			{
				return typename TProtocol::endpoint(*GetV4Addr(), port);
			}
			case EndPointParser::AtypIPv6:
			{
				return typename TProtocol::endpoint(*GetV6Addr(), port);
			}
			default:
				assert(false);
				break;
			}
			__assume(false);
		}
		template<typename TProtocol>
		typename TProtocol::resolver::query ParseDomain()
		{
			assert(aytp == EndPointParser::AtypDomainName);
			return typename TProtocol::resolver::query(*GetDomain(), _BOOST lexical_cast<_STD string>(port));
		}
		Atyp GetAtyp() const
		{
			return aytp;
		}
	private:
		typedef union
		{
			_ASIO ip::address_v4 ipv4;
			_ASIO ip::address_v6 ipv6;
			_STD string domain;
		}UnionType;
		enum
		{
			UnionTypeSize = sizeof(UnionType)
		};
		_STD array<unsigned char, UnionTypeSize> buffer;
		unsigned short port;
		Atyp aytp;
		unsigned char* GetBufferPtr()
		{
			return buffer.data();
		}
		const unsigned char* GetBufferPtr() const
		{
			return buffer.data();
		}
		const _ASIO ip::address_v4* GetV4Addr() const
		{
			return reinterpret_cast<const _ASIO ip::address_v4*>(GetBufferPtr());
		}
		const _ASIO ip::address_v6* GetV6Addr() const
		{
			return reinterpret_cast<const _ASIO ip::address_v6*>(GetBufferPtr());
		}
		const _STD string* GetDomain() const
		{
			return reinterpret_cast<const _STD string*>(GetBufferPtr());
		}

	};

	template<typename TSession>
	class DeadlineSession :public _BOOST noncopyable
	{
	public:
		DeadlineSession(shared_ptr<TSession>&& val) :
			timer(val->GetIoService()),
			session(val)
		{
		}

		~DeadlineSession()
		{
			//CancelTimer(_BOOST system::error_code());
		}
		void Close(_BOOST system::error_code& ignore)
		{
			CancelTimer(ignore);
			session->Close(ignore);
		}
		void CancelTimer(_BOOST system::error_code& ignore)
		{
			timer.cancel(ignore);
		}
		shared_ptr<TSession>& GetSession()
		{
			return session;
		}
		_ASIO deadline_timer& GetTimer()
		{
			return timer;
		}
	private:
		shared_ptr<TSession> session;
		_ASIO deadline_timer timer;
	};

	template<typename TSession>
	class TcpForward :public _BOOST noncopyable
	{
	public:
		TcpForward(shared_ptr<TSession>& read_, shared_ptr<TSession>& write_) :
			readBuffer(_ASIO buffer(read->GetBuffer())),
			read(read_),
			write(write_)
		{
		}

		_ASIO mutable_buffers_1& GetReadBuffer()
		{
			return readBuffer;
		}
		shared_ptr<TSession>& GetReadSession()
		{
			return read;
		}
		shared_ptr<TSession>& GetWriteSession()
		{
			return write;
		}
	private:
		shared_ptr<TSession> read;
		shared_ptr<TSession> write;
		_ASIO mutable_buffers_1 readBuffer;
	};
}  // namespace FeedTheDog
