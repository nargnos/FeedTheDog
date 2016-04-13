#pragma once
#include "ServiceBase.h"
#include "Rfc1928Def.h"
namespace FeedTheDog
{
	// FIX: 协商部分需要限制读取最大长度，防止恶意请求

	class Rfc1928 :
		public ServiceBase
	{
	public:
		enum : unsigned char
		{
			NoAuthenticationRequired = 0x00,
			Gssapi = 0x01,
			UsernamePassword = 0x02,
			IanaBegin = 0x03,
			IanaEnd = 0x7f,
			ReservedBegin = 0x80,
			ReservedEnd = 0xfe,
		};
		typedef TcpForward<TTcpSession> TTcpForward;
		typedef Async TUtil;
		typedef EndPointParser TEndPointParser;
		typedef DeadlineSession<TTcpSession> TTcpDeadlineSession;

		friend TTcpForward;


		Rfc1928(int port, const char* name, int timeout = 3);
		~Rfc1928();
		virtual void AsyncStart() override;

		virtual void Stop() override;
		virtual bool Init(TServiceManager *) override;
	private:
		bool isStopped;
		bool supportMethods[0xff];
		TServiceManager* manager;
		int port_;
		_BOOST system::error_code ignore;
		_BOOST posix_time::seconds timeoutSecond;
		unique_ptr<_ASIO ip::tcp::acceptor> acceptor;

		bool __fastcall CheckVersionMessage(size_t, const VersionMessage *);
		int __fastcall BuildCmdConnectReplyMessage(ServerReplieMessage *, shared_ptr<TTcpSession>&, const _BOOST system::error_code &);
		unsigned char __fastcall ReplySelectedMethod(_ASIO ip::tcp::socket&, VersionMessage *, _BOOST system::error_code &);
		void __fastcall CheckDeadline(shared_ptr<TTcpDeadlineSession>&, const _BOOST system::error_code&);
		void __fastcall ForwardRead(shared_ptr<TTcpForward>& forward, const _BOOST system::error_code & error, size_t bytes_transferred);
		void __fastcall ForwardWrite(shared_ptr<TTcpForward>& forward, const _BOOST system::error_code & error, size_t bytes_transferred);
		void __fastcall CreateForward(shared_ptr<TTcpSession>&, shared_ptr<TTcpSession>&);
		void __fastcall DoCmd(shared_ptr<TTcpDeadlineSession>&, unique_ptr<TEndPointParser>&, unsigned char);
		void __fastcall DoCmdConnect(shared_ptr<TTcpDeadlineSession>&, unique_ptr<TEndPointParser>&);
		void __fastcall HandleAccept(shared_ptr<TTcpDeadlineSession>&, const _BOOST system::error_code&);
		void __fastcall HandleCmdConnectReply(shared_ptr<TTcpDeadlineSession>&, shared_ptr<TTcpSession>&, const _BOOST system::error_code &);

		void __fastcall HandleNoAuthenticationRequired(shared_ptr<TTcpDeadlineSession>&, size_t, size_t, const _BOOST system::error_code &);
		void __fastcall HandleReadVersionMessage(shared_ptr<TTcpDeadlineSession>&, size_t, size_t, const _BOOST system::error_code &);
		void __fastcall HandleResolver(shared_ptr<TTcpDeadlineSession>&, const _ASIO ip::tcp::resolver::iterator &, const _BOOST system::error_code &);
		void __fastcall RunMethod(shared_ptr<TTcpDeadlineSession>&, unsigned char);
	};



}  // namespace FeedTheDog


