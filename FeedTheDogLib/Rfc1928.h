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
		typedef TcpForward<TcpSession> TTcpForward;
		typedef EndPointParser TEndPointParser;
		typedef DeadlineSession<TcpSession> TTcpDeadlineSession;


		Rfc1928(int port, const char* name, int timeout = 3);
		~Rfc1928();
		virtual void Start() override;

		virtual void Stop() override;

	private:
		bool supportMethods[0xff];
		_BOOST system::error_code ignore;
		_BOOST posix_time::seconds timeoutSecond;
		unique_ptr<_ASIO ip::tcp::acceptor> acceptor;
		int port_;

		bool isStopped;
		template<typename ReadHandler, typename TTcpSession>
		void TcpReadMore(shared_ptr<TTcpSession>& session, ReadHandler&& handler, size_t alreadyTransferred = 0, size_t maxSize = 0)
		{

			auto& buffer = session->GetBuffer();
			assert(maxSize <= buffer.max_size());
			auto bufferData = buffer.data();

			auto leftSize = (maxSize == 0 ? buffer.max_size() : maxSize) - alreadyTransferred;
			assert(leftSize > 0);
			session->AsyncReadSome(_ASIO buffer(bufferData + alreadyTransferred, leftSize), _STD forward<ReadHandler>(handler));
		}
		virtual bool InitService() override;
		static bool FASTCALL CheckVersionMessage(size_t, const VersionMessage *);
		int FASTCALL BuildCmdConnectReplyMessage(ServerReplieMessage *, shared_ptr<TcpSession>&, const _BOOST system::error_code &);
		void FASTCALL ReplySelectedMethod(shared_ptr<TTcpDeadlineSession>&, VersionMessage *);
		void FASTCALL CheckDeadline(shared_ptr<TTcpDeadlineSession>&, const _BOOST system::error_code&);
		void FASTCALL ForwardRead(shared_ptr<TTcpForward>& forward, const _BOOST system::error_code & error, size_t bytes_transferred);
		void FASTCALL ForwardWrite(shared_ptr<TTcpForward>& forward, const _BOOST system::error_code & error, size_t bytes_transferred);
		void FASTCALL CreateForward(shared_ptr<TcpSession>&, shared_ptr<TcpSession>&);
		void FASTCALL DoCmd(shared_ptr<TTcpDeadlineSession>&, unique_ptr<TEndPointParser>&, unsigned char);
		void FASTCALL DoCmdConnect(shared_ptr<TTcpDeadlineSession>&, unique_ptr<TEndPointParser>&);
		void FASTCALL HandleAccept(shared_ptr<TTcpDeadlineSession>&, const _BOOST system::error_code&);
		void FASTCALL HandleCmdConnectReply(shared_ptr<TTcpDeadlineSession>&, shared_ptr<TcpSession>&, const _BOOST system::error_code &);

		void FASTCALL HandleNoAuthenticationRequired(shared_ptr<TTcpDeadlineSession>&, size_t, size_t, const _BOOST system::error_code &);
		void FASTCALL HandleReadVersionMessage(shared_ptr<TTcpDeadlineSession>&, size_t, size_t, const _BOOST system::error_code &);
		void FASTCALL HandleResolver(shared_ptr<TTcpDeadlineSession>&, const _ASIO ip::tcp::resolver::iterator &, const _BOOST system::error_code &);
		void FASTCALL RunMethod(shared_ptr<TTcpDeadlineSession>&, unsigned char);
	};



}  // namespace FeedTheDog


