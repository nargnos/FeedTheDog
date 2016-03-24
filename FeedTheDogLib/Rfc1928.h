#pragma once
#include "ServiceBase.h"
#include "Rfc1928Def.h"
namespace FeedTheDog
{
	class Rfc1928;
	struct Rfc1928Trait
	{
		typedef TcpForward<Rfc1928> TTcpForward;
		typedef Async TUtil;
		typedef EndPointParser TEndPointParser;
		typedef DeadlineSession<_ASIO ip::tcp> TTcpDeadlineSession;
	};
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
		typedef Rfc1928Trait::TTcpForward TTcpForward;
		friend TTcpForward;
		typedef Rfc1928Trait::TUtil TUtil;
		typedef Rfc1928Trait::TEndPointParser TEndPointParser;
		typedef Rfc1928Trait::TTcpDeadlineSession TTcpDeadlineSession;


		Rfc1928(int port, const char* name, int timeout = 3);
		~Rfc1928();
		virtual void AsyncStart() override;

		virtual void Stop() override;
		virtual bool Init(TCore *) override;
	private:
		bool isStopped;
		bool supportMethods[0xff];
		TCore* core;
		int port_;
		_BOOST system::error_code ignore;
		_BOOST posix_time::seconds timeoutSecond;
		unique_ptr<_ASIO ip::tcp::acceptor> acceptor;

		bool CheckVersionMessage(size_t, const VersionMessage *);
		int BuildCmdConnectReplyMessage(ServerReplieMessage *, shared_ptr<TTcpSession>&, const _BOOST system::error_code &);
		unsigned char ReplySelectedMethod(_ASIO ip::tcp::socket&, VersionMessage *, _BOOST system::error_code &);
		void CheckDeadline(shared_ptr<TTcpDeadlineSession>&, const _BOOST system::error_code&);
		void CreateForward(shared_ptr<TTcpSession>&, shared_ptr<TTcpSession>&);
		void DoCmd(shared_ptr<TTcpDeadlineSession>&, unique_ptr<TEndPointParser>&, unsigned char);
		void DoCmdConnect(shared_ptr<TTcpDeadlineSession>&, unique_ptr<TEndPointParser>&);
		void HandleAccept(shared_ptr<TTcpDeadlineSession>&, const _BOOST system::error_code&);
		void HandleCmdConnectReply(shared_ptr<TTcpDeadlineSession>&, shared_ptr<TTcpSession>&, const _BOOST system::error_code &);

		void HandleNoAuthenticationRequired(shared_ptr<TTcpDeadlineSession>&, size_t, size_t, const _BOOST system::error_code &);
		void HandleReadVersionMessage(shared_ptr<TTcpDeadlineSession>&, size_t, size_t, const _BOOST system::error_code &);
		void HandleResolver(shared_ptr<TTcpDeadlineSession>&, const _ASIO ip::tcp::resolver::iterator &, const _BOOST system::error_code &);
		void RunMethod(shared_ptr<TTcpDeadlineSession>&, unsigned char);
	};



}  // namespace FeedTheDog


