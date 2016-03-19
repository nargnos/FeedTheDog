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
		typedef typename CoreTrait::TWorker TWorker;

		typedef typename TProtocol::socket TSocket;
		typedef typename _BOOST shared_mutex TMutex;
		typedef _BOOST shared_lock<TMutex> ReadLock;
		typedef _BOOST unique_lock<TMutex> WriteLock;
		typedef typename TProtocol::resolver TResolver;

		// session的析构委托
		typedef _BOOST _bi::bind_t<
			void, _BOOST _mfi::mf1<void, TSelf, TSession *>,
			_BOOST _bi::list2<_BOOST _bi::value<TSelf*>, _BOOST arg<1>>
		> TFreeSessionDelegate;

		SessionPool(TCore* core, TWorker* worker, _ASIO io_service& io) :
			corePtr(core),
			ios(io),
			count(0),
			sessionPool(alloc, 0),
			freeSessionDelegate(_BOOST bind(&TSelf::Free, this, _1)),
		//	strand(io),
			worker_(worker),
			resolver(io)
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::NewSessionPool);
			isDestruct = false;
		}

		~SessionPool()
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::FreeSessionPool);
			isDestruct = true;
			if (count > 0)
			{
				CloseAll();
			}
		}
		// 多线程同时new多次有一定概率出现线程争用
		virtual shared_ptr<TSession> NewSession(const char* serviceName) override
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::NewSession);
			++count;
			TSession* result = NULL;

			// FIX: 看有什么更快的方案可以换
			// 构建session
			result = sessionPool.construct<true, false>(worker_);

			// 析构的erase执行时间绝对会在insert执行之后，所以在析构时不会出现未设置map插入位置的情况
			ios.post(_BOOST bind(&TSelf::InsertSession, this, serviceName, result));

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
			
			ios.post(_BOOST bind(&TSelf::AsyncCloseAll, this));
		}

		// 移除服务session前先会停止服务，此时服务应拒绝新连接
		// 只在运行状态调用
		virtual void RemoveServiceSession(const char* serviceName) override
		{
			ios.post(_BOOST bind(&TSelf::RemoveServiceSessionStrand, this, serviceName));
		}
		TResolver& GetResolver()
		{
			return resolver;
		}
	private:
		TWorker* worker_;
		//_ASIO strand strand;
		// 表示是否处于析构状态
		bool isDestruct;
		// 简化调用bind
		TFreeSessionDelegate freeSessionDelegate;

		TCore* corePtr;
		_ASIO io_service& ios;
		_BOOST atomic<unsigned int> count;

		TResolver resolver;

		// 池(在freelock里找到的，是否好用待测试)
		_STD allocator<TSession> alloc;

		TPool sessionPool;
		// 用作分配对象的指针存储
		// FIX: 这里insert和erase耗费cpu非常多
		TMap sessionMap;

		void InsertSession(const char* serviceName, TSession* session)
		{
			session->mapPosition = sessionMap.insert(TMapValue(serviceName, session));
		}
		void AsyncCloseAll()
		{
			for each (auto& var in sessionMap)
			{
				var.second->Close();
			}
		}
		void FreeSession(TSession* ptr)
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::FreeSession);
			if (!ptr->isErased)
			{
				// 析构时不erase减少析构时间
				if (!isDestruct)
				{
					// FIX: cpu
					sessionMap.erase(ptr->mapPosition);
				}
				ptr->isErased = true;
			}
			// FIX: cpu
			sessionPool.destruct<true>(ptr);
			--count;
		}
		void RemoveServiceSessionStrand(const char* serviceName)
		{
			auto& it = sessionMap.find(serviceName);
			auto eraseIt = it;
			while (it != sessionMap.end())
			{
				it->second->Close();
				it->second->isErased = true;
			}
			sessionMap.erase(eraseIt, sessionMap.end());
		}
		void Free(TSession* ptr)
		{
			ios.post(_BOOST bind(&TSelf::FreeSession, this, ptr));
		}
	};

}  // namespace FeedTheDog