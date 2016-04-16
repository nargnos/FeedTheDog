#pragma once

#include "ServicePolicy.h"
namespace FeedTheDog
{
	template<typename TServicePolicy>
	class ServiceBaseImpl :
		public IService<typename TServicePolicy::TServiceManager>
	{
	public:
		typedef typename TServiceManager::TWorker TWorker;
		typedef typename TServiceManager::TTraceSource TTraceSource;
		typedef typename TWorker::TTcp TTcp;
		typedef typename TWorker::TUdp TUdp;
		typedef typename TWorker::TTcpSessionPool TTcpSessionPool;
		typedef typename TWorker::TTcpSession TTcpSession;
		typedef typename TWorker::TUdpSessionPool TUdpSessionPool;
		typedef typename TWorker::TUdpSession TUdpSession;
		typedef typename TServiceManager::TTraceSource::TLevel TLevel;
		typedef typename TServicePolicy::TSessionManager TSessionManager;

		template<typename TProtocol>
		struct TSession
		{
			typedef typename TWorker::template TSession<TProtocol>::TSessionType TSessionType;
		};

		ServiceBaseImpl(const char* name)
		{
			isInitialized = false;
			name_ = name;
			isStopped = false;
		}
		virtual const char* Name() const override
		{
			return name_;
		}
		virtual bool Init(TServiceManager* managerPtr) override
		{
			assert(!isInitialized);
			manager = managerPtr;
			isStopped = false;
			isInitialized = true;
			return InitService();
		}
		inline TWorker* SelectIdleWorker()
		{
			assert(isInitialized);
			return manager->SelectIdleWorker();
		}
		inline unique_ptr<TTraceSource>& GetTrace()
		{
			return manager->GetTrace();
		}
		// FIX: 以下那些函数是用来扩展Encryptor的，用一个中间层来加解密，但是感觉这样做不太好，socket函数太多了不能完全导出，
		// 而且为了调用方式上的一致，解析和连接方面也要导出，这样用不太方便，好处是可以对session做各种扩展
		// 第二个方法是在Session类里做函数转发，这样在每次发送时需要传入IEncryptor做加密策略，最少也要写一遍函数导出
		// 缺点是如果再扩展其它非加密策略的话就要修改接口，用装饰的话也要传入东西，这样创建一个session就要new几个对象性能会有影响
		// 好处是直观使用方便
		// 两种都需要把socket函数导出，如果在所有函数发送、接收前hook，就不用导出任何函数，所有的都按默认方式使用，找找文档看看有没有什么方法

		// Tcp
		template <typename MutableBufferSequence, typename ReadHandler>
		inline void AsyncReadSome(TTcpSession& session,
			const MutableBufferSequence& buffers,
			ReadHandler&& handler)
		{
			session.GetSocket().async_read_some(buffers, _STD forward<ReadHandler>(handler));
		}

		// Tcp
		template <typename MutableBufferSequence, typename WriteHandler>
		inline void AsyncWriteSome(TTcpSession& session,
			const MutableBufferSequence& buffers,
			WriteHandler&& handler)
		{
			session.GetSocket().async_write_some(buffers, _STD forward<WriteHandler>(handler));
		}

		// Udp
		template <typename MutableBufferSequence, typename ReadHandler>
		inline void AsyncReadSome(TUdpSession& session,
			typename TUdp::endpoint& endpoint,
			const MutableBufferSequence& buffers,
			ReadHandler&& handler)
		{
			session.GetSocket().async_receive_from(buffers, endpoint, _STD forward<ReadHandler>(handler));
		}
		// Udp
		template <typename MutableBufferSequence, typename WriteHandler>
		inline void AsyncWriteSome(TUdpSession& session,
			typename TUdp::endpoint& endpoint,
			const MutableBufferSequence& buffers,
			WriteHandler&& handler)
		{
			session.GetSocket().async_send_to(buffers, endpoint, _STD forward<WriteHandler>(handler));
		}


		// Tcp 的一些函数
		template <typename TSession, typename MutableBufferSequence, typename ReadHandler>
		inline void AsyncRead(TSession& session,
			const MutableBufferSequence& buffers, ReadHandler&& handler)
		{
			_ASIO async_read(session.GetSocket(), buffers, _STD forward<ReadHandler>(handler));
		}



		template <typename TSession, typename MutableBufferSequence, typename WriteHandler>
		inline void AsyncWrite(TSession& session,
			const MutableBufferSequence& buffers, WriteHandler&& handler)
		{
			_ASIO async_write(session.GetSocket(), buffers, _STD forward<WriteHandler>(handler));
		}



		template <typename TSession, typename MutableBufferSequence>
		inline _STD size_t Read(TSession& session,
			const MutableBufferSequence& buffers, _BOOST system::error_code& ec)
		{
			return _ASIO read(session.GetSocket(), buffers, ec);
		}

		template <typename TSession, typename ConstBufferSequence>
		inline _STD size_t Write(TSession& session,
			const ConstBufferSequence& buffers, _BOOST system::error_code& ec)
		{
			return _ASIO write(session.GetSocket(), buffers, ec);
		}

		template <typename TSession, typename Iterator, typename ComposedConnectHandler>
		inline void AsyncConnect(TSession& session, Iterator begin, ComposedConnectHandler&& handler)
		{
			_ASIO async_connect(session.GetSocket(), begin, _STD forward<ComposedConnectHandler>(handler));
		}

		template <typename TSession, typename ConnectHandler>
		inline void AsyncConnect(TSession& session,
			const typename TTcp::endpoint& peer_endpoint,
			ConnectHandler&& handler)
		{
			session.GetSocket().async_connect(peer_endpoint, _STD forward<ConnectHandler>(handler));
		}

		template <typename TSession, typename TAcceptor, typename AcceptHandler>
		inline void AsyncAccept(TAcceptor& acceptor,TSession& session, AcceptHandler&& handler)
		{
			acceptor.async_accept(session.GetSocket(), _STD forward<AcceptHandler>(handler));
		}

	protected:
		virtual bool InitService() = 0;

		const char* name_;
		bool isStopped;
	private:
		bool isInitialized;
		TServiceManager* manager;
		TSessionManager sessionManager;
	};
	typedef ServiceBaseImpl<ServicePolicy> ServiceBase;
}  // namespace FeedTheDog
