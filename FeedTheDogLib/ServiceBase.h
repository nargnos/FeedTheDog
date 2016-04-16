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
		// FIX: ������Щ������������չEncryptor�ģ���һ���м�����ӽ��ܣ����Ǹо���������̫�ã�socket����̫���˲�����ȫ������
		// ����Ϊ�˵��÷�ʽ�ϵ�һ�£����������ӷ���ҲҪ�����������ò�̫���㣬�ô��ǿ��Զ�session��������չ
		// �ڶ�����������Session����������ת����������ÿ�η���ʱ��Ҫ����IEncryptor�����ܲ��ԣ�����ҲҪдһ�麯������
		// ȱ�����������չ�����Ǽ��ܲ��ԵĻ���Ҫ�޸Ľӿڣ���װ�εĻ�ҲҪ���붫������������һ��session��Ҫnew�����������ܻ���Ӱ��
		// �ô���ֱ��ʹ�÷���
		// ���ֶ���Ҫ��socket������������������к������͡�����ǰhook���Ͳ��õ����κκ��������еĶ���Ĭ�Ϸ�ʽʹ�ã������ĵ�������û��ʲô����

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


		// Tcp ��һЩ����
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
