#pragma once
#include "SessionPoolTrait.h"
#include "CoreTrait.h"

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

		SessionPool(const shared_ptr<TCore>& core, _ASIO io_service& io) :
			ios(io),
			count(0),
			tcpSessionPool(tcpAlloc, 0),
			udpSessionPool(udpAlloc, 0)
		{
			static int sid = 0;
			id = sid++;
			weakcore = core;
		}

		virtual ~SessionPool()
		{
			if (count>0)
			{
				CloseAll();
			}
			assert(count == 0);
		}
		
		long long GetSessionCount() const
		{
			return count;
		}
		// 有在多个线程分配的情况（accept回调时在当前线程创建属于另一worker的session）
		template<typename TProtocol>
		shared_ptr<typename SessionPoolTrait::TSession<TProtocol>::type> NewSession()
		{
			auto result = GetSessionPool<TProtocol>().construct<true, false>(weakcore.lock(), _STD ref(ios));

			GetSet<TProtocol>().insert(result);
			++count;
			return shared_ptr<typename SessionPoolTrait::TSession<TProtocol>::type>(
				result,
				_BOOST bind(&SessionPool::Free<TProtocol>, this, _1));
		}
		void CloseAll()
		{
			CloseAll<TTcp>();
			CloseAll<TUdp>();
		}
	private:
		_BOOST mutex mutex;
		template<typename TProtocol>
		void CloseAll()
		{
			auto& set = GetSet<TProtocol>();
			int tmpCount = set.size();
			for each (auto& var in set)
			{
				auto& socket = var->GetSocket();
				if(socket.is_open())
				{
					socket.close();
				}
			}
		}

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
			auto& socket =ptr->GetSocket();
			if (socket.is_open())
			{
				socket.close();
			}
			set.unsafe_erase(ptr);
			mutex.unlock();
			GetSessionPool<TProtocol>().destruct<true>(ptr);
			--count;
		}

		_BOOST atomic<long long> count;

		_STD allocator<TTcpSession> tcpAlloc;
		_STD allocator<TUdpSession> udpAlloc;

		typename SessionPoolTrait::TPoolType<TTcp>::type tcpSessionPool;
		typename SessionPoolTrait::TPoolType<TUdp>::type udpSessionPool;

		concurrent_unordered_set<TTcpSession*> tcpSessions;
		concurrent_unordered_set<TUdpSession*> udpSessions;
		int id;
		weak_ptr<TCore> weakcore;
		_ASIO io_service& ios;
	};
}  // namespace FeedTheDog