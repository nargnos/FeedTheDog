#pragma once
#include "SessionPoolTrait.h"
#include "CoreTrait.h"
#include "Core.h"
#include "Session.h"
namespace FeedTheDog
{
	class SessionPool :
		public _STD enable_shared_from_this<SessionPool>
	{
	public:
		typedef typename SessionPoolTrait::TTcp TTcp;
		typedef typename SessionPoolTrait::TUdp TUdp;
		typedef typename SessionPoolTrait::TTcpSession TTcpSession;
		typedef typename SessionPoolTrait::TUdpSession TUdpSession;
		typedef typename CoreTrait::TCore TCore;

		SessionPool(TCore* core, _ASIO io_service& io) :
			ios(io),
			count(0),
			tcpSessionPool(tcpAlloc, 0),
			udpSessionPool(udpAlloc, 0),
			tcpfreeSessionDelegate(_BOOST bind(&SessionPool::Free<TTcp>, this, _1)),
			udpfreeSessionDelegate(_BOOST bind(&SessionPool::Free<TUdp>, this, _1))
		{
			static unsigned char sid = 0;
			id = sid++;
			corePtr = core;
			corePtr->GetTrace()->TracePoint(LogMsg::NewSessionPool, true, id);
			//const int hhhhh=sizeof(typename SessionPoolTrait::TSession<TTcp>::type);

		}

		virtual ~SessionPool()
		{
			if (count > 0)
			{
				CloseAll();
			}
			assert(count == 0);
			corePtr->GetTrace()->TracePoint(LogMsg::FreeSessionPool, true, id);
		}

		unsigned int GetSessionCount() const
		{
			return count;
		}
		// ���ڶ���̷߳���������accept�ص�ʱ�ڵ�ǰ�̴߳���������һworker��session��
		template<typename TProtocol>
		shared_ptr<typename SessionPoolTrait::TSession<TProtocol>::type> NewSession(const char* serviceName)
		{
#ifdef _DEBUG
			corePtr->GetTrace()->TracePoint(LogMsg::AllocMemory, true, id, serviceName);
#endif // _DEBUG
			// ͳ��session
			++count;
			typedef typename SessionPoolTrait::TSessionSave<TProtocol>::MapValue TValue;

			// ����session���ṹ���̰߳�ȫ�ľͲ�lock��
			auto result = GetSessionPool<TProtocol>().construct<true, false>(corePtr, _STD ref(ios));
			// �������λ�ã�ɾ��ʱʹ��
			// lock ���ﲻ����������,����Ϊ�̰߳�ȫ
			ReadLock lock(mutex);
			result->mapPosition = GetSet<TProtocol>().insert(TValue(serviceName, result));
			return shared_ptr<typename SessionPoolTrait::TSession<TProtocol>::type>(result, GetFreeSessionDelegate<TProtocol>());
		}
		void CloseAll()
		{
			CloseAll<TTcp>();
			CloseAll<TUdp>();
		}
		// �ص���ɾ����������ָ��free�е��ظ�ɾ�����������
		void RemoveServiceSession(const char* serviceName)
		{
			RemoveServiceSession<TTcp>(serviceName);
			RemoveServiceSession<TUdp>(serviceName);
		}
	private:
		// �򻯵���bind
#pragma region bind
		template<typename TProtocol>
		struct TFreeSessionDelegate
		{
			typedef _BOOST _bi::bind_t<
				void, _BOOST _mfi::mf1<void, SessionPool, Session<TProtocol> *>,
				_BOOST _bi::list2<_BOOST _bi::value<SessionPool*>, _BOOST arg<1>>
			> type;
		};
		typename TFreeSessionDelegate<TTcp>::type tcpfreeSessionDelegate;
		typename TFreeSessionDelegate<TUdp>::type udpfreeSessionDelegate;

		template<typename TProtocol>
		typename TFreeSessionDelegate<TProtocol>::type& GetFreeSessionDelegate();
		template<>
		typename TFreeSessionDelegate<TTcp>::type& GetFreeSessionDelegate<TTcp>()
		{
			return tcpfreeSessionDelegate;
		}
		template<>
		typename TFreeSessionDelegate<TUdp>::type& GetFreeSessionDelegate <TUdp>()
		{
			return udpfreeSessionDelegate;
		}
#pragma endregion

#pragma region lock
		typedef _BOOST shared_lock<_BOOST shared_mutex> ReadLock;
		typedef _BOOST unique_lock<_BOOST shared_mutex> WriteLock;
		_BOOST shared_mutex mutex;
#pragma endregion
		// ��(��freelock���ҵ��ģ��Ƿ���ô�����)��session�洢
#pragma region pool set
		_STD allocator<TTcpSession> tcpAlloc;
		_STD allocator<TUdpSession> udpAlloc;
		// pool
		typename SessionPoolTrait::TPoolType<TTcp>::type tcpSessionPool;
		typename SessionPoolTrait::TPoolType<TUdp>::type udpSessionPool;
		// ������������ָ��洢
		typename SessionPoolTrait::TSessionSave<TTcp>::MapType tcpSessions;
		typename SessionPoolTrait::TSessionSave<TUdp>::MapType udpSessions;
		template<typename TProtocol>
		inline typename SessionPoolTrait::TSessionSave<TProtocol>::MapType& GetSet();

		template<>
		inline typename SessionPoolTrait::TSessionSave<TTcp>::MapType& GetSet<TTcp>()
		{
			return tcpSessions;
		}
		template<>
		inline typename SessionPoolTrait::TSessionSave<TUdp>::MapType& GetSet<TUdp>()
		{
			return udpSessions;
		}
		template<typename TProtocol>
		inline typename SessionPoolTrait::TPoolType<TProtocol>::type& GetSessionPool();

		template<>
		inline typename SessionPoolTrait::TPoolType<TTcp>::type& GetSessionPool<TTcp>()
		{
			return tcpSessionPool;
		}
		template<>
		inline typename SessionPoolTrait::TPoolType<TUdp>::type& GetSessionPool<TUdp>()
		{
			return udpSessionPool;
		}
#pragma endregion
		// ͳ��session����
		_BOOST atomic<unsigned int> count;

		unsigned char id;
		TCore* corePtr;
		_ASIO io_service& ios;

		template<typename TProtocol>
		void CloseAll()
		{
			// ֻ�ص����ӣ�ɾ������������ָ�봦��
			auto& set = GetSet<TProtocol>();
#ifdef _DEBUG
			auto str = std::is_same<TTcp, TProtocol>::value ? "Tcp" : "Udp";
			corePtr->GetTrace()->TracePoint(LogMsg::CloseAllSocket, true, id, str);
#endif // _DEBUG

			// lock
			ReadLock lock(mutex);
			for each (auto& var in set)
			{
				CloseSession<TProtocol>(var.second);
			}
		}

		// �Ƴ�����sessionǰ�Ȼ�ֹͣ���񣬴�ʱ����Ӧ�ܾ�������
		template<typename TProtocol>
		void RemoveServiceSession(const char* serviceName)
		{
			// ��ʱ��������Ż��ռ�
			auto& set = GetSet<TProtocol>();
			// lock
			ReadLock readlock(mutex);
			auto& it = set.find(serviceName);
			while (it != set.end())
			{
				CloseSession<TProtocol>(it->second);
				it->second->isErased = true;
			}
			readlock.unlock();
			// lock
			WriteLock lock(mutex);
			set.unsafe_erase(serviceName);
		}


		template<typename TProtocol>
		void CloseSession(typename SessionPoolTrait::TSession<TProtocol>::type* ptr)
		{
			if (!ptr->isClosed)
			{
				ptr->isClosed = true;
				auto& socket = ptr->GetSocket();
				socket.shutdown(_ASIO socket_base::shutdown_type::shutdown_both);
				socket.close();
			}
		}

		template<typename TProtocol>
		void Free(typename SessionPoolTrait::TSession<TProtocol>::type* ptr)
		{
			// lock
			WriteLock lock(mutex);
			if (!ptr->isErased)
			{
				auto& set = GetSet<TProtocol>();
				set.unsafe_erase(ptr->mapPosition);
				ptr->isErased = true;
			}
			lock.unlock();
#ifdef _DEBUG
			corePtr->GetTrace()->TracePoint(LogMsg::FreeMemory, true, id);
#endif // _DEBUG
			GetSessionPool<TProtocol>().destruct<true>(ptr);
			--count;
		}
	};
}  // namespace FeedTheDog