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
		private _BOOST noncopyable
	{
	public:
		typedef typename SessionPool<TProtocol> TSelf;
		typedef typename SessionPoolTrait::TSession<TProtocol>::type TSession;
		typedef typename SessionPoolTrait::TPool<TProtocol>::type TPool;
		typedef typename SessionPoolTrait::TSessionStorage<TProtocol>::type TStorage;
		typedef typename TStorage::iterator TStorageIterator;
		typedef typename TStorage::value_type TStorageValue;
		typedef typename CoreTrait::TCore TCore;
		typedef typename CoreTrait::TWorker TWorker;

		typedef typename TProtocol::socket TSocket;
		//typedef typename _BOOST shared_mutex TMutex;
		//typedef _BOOST shared_lock<TMutex> ReadLock;
		//typedef _BOOST unique_lock<TMutex> WriteLock;
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
			assert(count == 0);
		}
		// 多线程同时new多次有一定概率出现线程争用
		shared_ptr<TSession> NewSession()
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::NewSession);
			++count;
			// FIX: 看有什么更快的方案可以换
			// 构建session
			TSession* result = sessionPool.construct<true, false>(worker_);

			// 析构的erase执行时间绝对会在insert执行之后，所以在析构时不会出现未设置插入位置的情况
			ios.post(_BOOST bind(&TSelf::InsertSession, this, result));

			return shared_ptr<TSession>(result, freeSessionDelegate);
		}


		unsigned int GetSessionCount() const
		{
			return count;
		}
		// 可在运行时和停止前调用
		void CloseAll()
		{
			// 只关掉连接，删除对象由智能指针处理			
			corePtr->GetTrace()->DebugPoint(LogMsg::CloseAllSocket);
			if (isDestruct)
			{
				AsyncCloseAll();
				return;
			}
			ios.post(_BOOST bind(&TSelf::AsyncCloseAll, this));
		}

		// 移除服务session前先会停止服务，此时服务应拒绝新连接
		// 只在运行状态调用
		/*void RemoveServiceSession(const char* serviceName)
		{
			ios.post(_BOOST bind(&TSelf::RemoveServiceSessionStrand, this, serviceName));
		}*/
		TResolver& GetResolver()
		{
			return resolver;
		}
		void __PreDestruct()
		{
			isDestruct = true;
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
		_BOOST system::error_code ignore;

		_STD allocator<TSession> alloc;
		// FIX:池(在freelock里找到的，是否好用待测试)
		TPool sessionPool;
		// 用作分配对象的指针存储

		TStorage sessionStorage;

		void InsertSession(TSession* session)
		{
			session->insertPosition = _STD move(sessionStorage.insert(sessionStorage.end(), session));
		}
		void AsyncCloseAll()
		{
			for each (auto& var in sessionStorage)
			{
				var->close(ignore);
			}
		}
		void FreeSession(TSession* ptr)
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::FreeSession);

			// 析构时不erase减少析构时间
			if (!isDestruct)
			{
				sessionStorage.erase(ptr->insertPosition);
			}

			// FIX: cpu
			// 在这种情况下使用lockfree可能会拖慢速度
			sessionPool.destruct<true>(ptr);
			--count;
		}
		/*	void RemoveServiceSessionStrand(const char* serviceName)
			{
				auto& it = sessionStorage.find(serviceName);
				auto eraseIt = it;
				while (it != sessionStorage.end())
				{
					it->second->close(ignore);
				}
				sessionStorage.erase(eraseIt, sessionStorage.end());
			}*/
		void Free(TSession* ptr)
		{
			if (isDestruct)
			{
				// ioservice会在free之前停止，所以不能post
				FreeSession(ptr);
				return;
			}
			ios.post(_BOOST bind(&TSelf::FreeSession, this, ptr));
		}
	};

}  // namespace FeedTheDog