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
			udpSessionPool(udpAlloc, 0)
		{
			static int sid = 0;
			id = sid++;
			corePtr = core;
			corePtr->GetTrace()->TracePoint(LogMsg::NewSessionPool, true, id);
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

		long long GetSessionCount() const
		{
			return count;
		}
		// 有在多个线程分配的情况（accept回调时在当前线程创建属于另一worker的session）
		template<typename TProtocol>
		shared_ptr<typename SessionPoolTrait::TSession<TProtocol>::type> NewSession(const char* serviceName)
		{
#ifdef _DEBUG
			corePtr->GetTrace()->TracePoint(LogMsg::AllocMemory, true, id, serviceName);
#endif // _DEBUG
			typedef typename SessionPoolTrait::TSessionSave<TProtocol>::MapValue TValue;

			// 结构都是线程安全的就不lock了
			auto result = GetSessionPool<TProtocol>().construct<true, false>(corePtr->shared_from_this(), _STD ref(ios));
			// 保存插入位置，删除时使用
			result->mapPosition = GetSet<TProtocol>().insert(TValue(serviceName, result));
			// 统计session
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
		// 关掉并删除相关项，智能指针free中的重复删除不会出问题
		void CloseServiceSession(const char* serviceName)
		{			
			CloseServiceSession<TTcp>(serviceName);
			CloseServiceSession<TUdp>(serviceName);
		}
	private:
		_BOOST mutex mutex;
		_BOOST atomic<long long> count;

		_STD allocator<TTcpSession> tcpAlloc;
		_STD allocator<TUdpSession> udpAlloc;
		// pool
		typename SessionPoolTrait::TPoolType<TTcp>::type tcpSessionPool;
		typename SessionPoolTrait::TPoolType<TUdp>::type udpSessionPool;
		// 用作分配对象的指针存储
		typename SessionPoolTrait::TSessionSave<TTcp>::MapType tcpSessions;
		typename SessionPoolTrait::TSessionSave<TUdp>::MapType udpSessions;



		int id;
		TCore* corePtr;
		_ASIO io_service& ios;

		template<typename TProtocol>
		typename SessionPoolTrait::TSessionSave<TProtocol>::MapType& GetSet();

		template<>
		typename SessionPoolTrait::TSessionSave<TTcp>::MapType& GetSet<TTcp>()
		{
			return tcpSessions;
		}
		template<>
		typename SessionPoolTrait::TSessionSave<TUdp>::MapType& GetSet<TUdp>()
		{
			return udpSessions;
		}

		template<typename TProtocol>
		void CloseAll()
		{
			// 只关掉连接，删除对象由智能指针处理
#ifdef _DEBUG
			auto str = std::is_same<TTcp, TProtocol>::value ? "Tcp" : "Udp";
			corePtr->GetTrace()->TracePoint(LogMsg::CloseAllSocket, true, id, str);
#endif // _DEBUG

			mutex.lock();

			auto& set = GetSet<TProtocol>();
			for each (auto& var in set)
			{
				auto& socket = var.second->GetSocket();
				if (socket.is_open())
				{
					socket.shutdown(_ASIO socket_base::shutdown_type::shutdown_both);
					socket.close();
				}
			}
			mutex.unlock();
		}
		template<typename TProtocol>
		void CloseServiceSession(const char* serviceName)
		{
			// FIX: 锁时间过长
			mutex.lock();
			auto& it = tcpSessions.find(serviceName);
			while (it!=tcpSessions.end())
			{
				auto& socket = it->second->GetSocket();
				if (socket.is_open())
				{
					socket.shutdown(_ASIO socket_base::shutdown_type::shutdown_both);
					socket.close();
				}
			}
			tcpSessions.unsafe_erase(serviceName);
			mutex.unlock();
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
		void Free(typename SessionPoolTrait::TSession<TProtocol>::type* ptr)
		{

			mutex.lock();
			auto& set = GetSet<TProtocol>();
			set.unsafe_erase(ptr->mapPosition);
			mutex.unlock();
#ifdef _DEBUG
			corePtr->GetTrace()->TracePoint(LogMsg::FreeMemory, true, id);
#endif // _DEBUG
			GetSessionPool<TProtocol>().destruct<true>(ptr);
			--count;
		}
	};
}  // namespace FeedTheDog