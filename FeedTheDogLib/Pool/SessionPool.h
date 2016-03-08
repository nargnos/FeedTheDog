#pragma once
#include "..\Common.h"
#include "..\Trait\SessionPoolTrait.h"
#include "..\Trait\CoreTrait.h"
#include "..\Core\Core.h"
#include "..\Core\Worker.h"
namespace FeedTheDog
{
	class SessionPool:
		public _STD enable_shared_from_this<SessionPool>
	{
	public:
		typedef typename SessionPoolTrait::TTcp TTcp;
		typedef typename SessionPoolTrait::TUdp TUdp;
		typedef typename SessionPoolTrait::TTcpSession TTcpSession;
		typedef typename SessionPoolTrait::TUdpSession TUdpSession;
		typedef typename CoreTrait::TCore TCore;
		typedef typename CoreTrait::TWorker TWorker;

		SessionPool(TWorker* worker) :
			count(0),
			tcpSessionPool(tcpAlloc, 0),
			udpSessionPool(udpAlloc, 0)
		{
			static int sid = 0;
			id=sid++;
			ptr = worker;
		}

		virtual ~SessionPool()
		{
			if (count > 0)
			{
				DestructAll<TTcp>();
				DestructAll<TUdp>();
			}
			assert(count == 0);
		}
		template<typename TProtocol>
		void DestructAll()
		{
			mutex.lock();
			auto& set = GetSet<TProtocol>();
			int tmpCount = set.size();
			for each (auto& var in set)
			{
				GetSessionPool<TProtocol>().destruct<true>(var);
			}
			set.clear();
			count -= tmpCount;
			mutex.unlock();
		}
		long long GetSessionCount() const
		{
			return count;
		}
		// 有在多个线程分配的情况（accept回调时在当前线程创建属于另一worker的session）
		template<typename TProtocol>
		shared_ptr<typename SessionPoolTrait::TSession<TProtocol>::type> NewSession()
		{
			auto result = GetSessionPool<TProtocol>().construct<true, false>(this,_STD ref(GetIoService()));

			GetSet<TProtocol>().insert(result);
			++count;
			return shared_ptr<typename SessionPoolTrait::TSession<TProtocol>::type>(
				result,
				_BOOST bind(&SessionPool::Free<TProtocol>, this, _1));
		}
		_ASIO io_service& GetIoService()
		{
			return *ptr->GetIoService();
		}
		// 取得拥有此对象池的Worker
		TWorker* GetWorker() const
		{
			return ptr;
		}
	private:
		_BOOST mutex mutex;
		template<typename TProtocol>
		typename SessionPoolTrait::TPoolType<TProtocol>::type& GetSessionPool();

		template<>
		typename SessionPoolTrait::TPoolType<TTcp>::type& GetSessionPool<TTcp>()
		{
			return tcpSessionPool;
		}
		template<>
		typename SessionPoolTrait::TPoolType<TUdp>::type& GetSessionPool<TUdp>()
		{
			return udpSessionPool;
		}

		template<typename TProtocol>
		concurrent_unordered_set<typename SessionPoolTrait::TSession<TProtocol>::type*>& GetSet();

		template<>
		concurrent_unordered_set<TTcpSession*>& GetSet<TTcp>()
		{
			return tcpSessions;
		}
		template<>
		concurrent_unordered_set<TUdpSession*>& GetSet<TUdp>()
		{
			return udpSessions;
		}

		template<typename TProtocol>
		void Free(typename SessionPoolTrait::TSession<TProtocol>::type* ptr)
		{
			auto& set = GetSet<TProtocol>();
			if (set.count(ptr) == 0)
			{
				return;
			}
			mutex.lock();
			set.unsafe_erase(ptr);
			mutex.unlock();
			// 保护pool不被析构
			auto pool = ptr->pool;
			GetSessionPool<TProtocol>().destruct<true>(ptr);			
			--count;
			pool.reset();
		}
		
		_BOOST atomic<long long> count;

		_STD allocator<TTcpSession> tcpAlloc;
		_STD allocator<TUdpSession> udpAlloc;

		typename SessionPoolTrait::TPoolType<TTcp>::type tcpSessionPool;
		typename SessionPoolTrait::TPoolType<TUdp>::type udpSessionPool;

		concurrent_unordered_set<TTcpSession*> tcpSessions;
		concurrent_unordered_set<TUdpSession*> udpSessions;
		int id;
		TWorker* ptr;
	};
}  // namespace FeedTheDog