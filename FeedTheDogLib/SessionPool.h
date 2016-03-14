#pragma once
#include "SessionPoolTrait.h"
#include "CoreTrait.h"
#include "ISessionPool.h"
#include "Core.h"
#include "Session.h"
namespace FeedTheDog
{
	template<typename TProtocol>
	class SessionPool :
		public ISessionPool<TProtocol>
	{
	public:
		typedef typename SessionPool<TProtocol> TSelf;
		typedef typename SessionPoolTrait::TSession<TProtocol>::type TSession;
		typedef typename SessionPoolTrait::TPool<TProtocol>::type TPool;
		typedef typename SessionPoolTrait::TSessionMultiMap<TProtocol>::type TMap;
		typedef typename TMap::iterator TMapIterator;
		typedef typename TMap::value_type TMapValue;
		typedef typename CoreTrait::TCore TCore;
		typedef typename TProtocol::socket TSocket;
		typedef typename _BOOST shared_mutex TMutex;
		typedef _BOOST shared_lock<TMutex> ReadLock;
		typedef _BOOST unique_lock<TMutex> WriteLock;

		// session的析构委托
		typedef _BOOST _bi::bind_t<
			void, _BOOST _mfi::mf1<void, TSelf, TSession *>,
			_BOOST _bi::list2<_BOOST _bi::value<TSelf*>, _BOOST arg<1>>
		> TFreeSessionDelegate;

		SessionPool(TCore* core, _ASIO io_service& io) :
			corePtr(core),
			ios(io),
			count(0),
			sessionPool(alloc, 0),
			freeSessionDelegate(_BOOST bind(&TSelf::Free, this, _1))
		{
			isDestruct = false;
		}

		~SessionPool()
		{
			isDestruct = true;
			if (count > 0)
			{
				CloseAll();
			}
		}

		// 有在多个线程分配的情况（accept回调时在当前线程创建属于另一worker的session）
		virtual shared_ptr<TSession> NewSession(const char* serviceName) override
		{
			++count;
			TSession* result = NULL;

			// 构建session
			result = sessionPool.construct<true, false>(corePtr, _STD ref(ios));

			// 可多线程同时insert，但是不能在erase同时插入，所以用sharedlock，别名为readlock
			ReadLock lock(mutex);
			// 保存插入位置，删除时使用
			// FIX: 这里消耗cpu
			result->mapPosition = sessionMap.insert(TMapValue(serviceName, result));
			return shared_ptr<TSession>(result, freeSessionDelegate);
		}
		virtual unsigned int GetSessionCount() const override
		{
			return count;
		}
		// 可在运行时和停止前调用
		virtual void CloseAll() override
		{
			// 只关掉连接，删除对象由智能指针处理			
#ifdef _DEBUG
			corePtr->GetTrace()->TracePoint(LogMsg::CloseAllSocket);
#endif // _DEBUG
			// lock
			ReadLock lock(mutex);
			for each (auto& var in sessionMap)
			{
				CloseSession(var.second);
			}
		}
		// 移除服务session前先会停止服务，此时服务应拒绝新连接
		// 只在运行状态调用
		virtual void RemoveServiceSession(const char* serviceName) override
		{
			// lock
			ReadLock readlock(mutex);
			auto& it = sessionMap.find(serviceName);
			while (it != sessionMap.end())
			{
				CloseSession(it->second);
				it->second->isErased = true;
			}
			readlock.unlock();
			// lock
			WriteLock lock(mutex);
			sessionMap.unsafe_erase(serviceName);
		}
	private:
		// 表示是否处于析构状态
		bool isDestruct;
		// 简化调用bind
		TFreeSessionDelegate freeSessionDelegate;

		TCore* corePtr;
		_ASIO io_service& ios;
		_BOOST atomic<unsigned int> count;

		// lock		
		_BOOST shared_mutex mutex;

		// 池(在freelock里找到的，是否好用待测试)
		_STD allocator<TSession> alloc;

		TPool sessionPool;
		// 用作分配对象的指针存储
		// FIX: 这里insert和erase耗费cpu非常多
		TMap sessionMap;

		void CloseSession(TSession* ptr)
		{
			if (!ptr->isClosed)
			{
				ptr->isClosed = true;
				auto& socket = ptr->GetSocket();
				socket.shutdown(_ASIO socket_base::shutdown_type::shutdown_both);
				socket.close();
			}
		}
		void Free(TSession* ptr)
		{
			// lock
			WriteLock lock(mutex);
			if (!ptr->isErased)
			{
				// 析构时不erase减少析构时间
				if (!isDestruct)
				{
					// FIX: cpu
					sessionMap.unsafe_erase(ptr->mapPosition);
				}
				ptr->isErased = true;
			}
			lock.unlock();
			// FIX: cpu
			sessionPool.destruct<true>(ptr);
			--count;
		}
		void DestructSession(TSession* ptr)
		{
			sessionPool.destruct<true>(ptr);
		}
	};

}  // namespace FeedTheDog