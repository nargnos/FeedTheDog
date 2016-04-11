#pragma once
#include "Session.h"
namespace FeedTheDog
{
	template<typename TProtocol,
		typename TOwner,
		typename Trait,
		typename TMemoryPool,
		typename TSessionStorage>
	class SessionPool :
		private _BOOST noncopyable
	{
	public:
		typedef typename Trait::template TSessionPool<TProtocol,TOwner, TMemoryPool, TSessionStorage>::TSessionType TSession;

		typedef typename TMemoryPool::template MemoryPool<TSession> TMemoryPool;
		typedef typename TMemoryPool::TPoolPtr TPoolPtr;

		typedef typename TSessionStorage::template SessionStorage<TSession> TStorage;
		typedef typename TStorage::TStoragePtr TStoragePtr;

		typedef typename TStorage::TStorageIterator TStorageIterator;
		typedef typename TStorage::TStorageValue TStorageValue;

		typedef typename TProtocol::socket TSocket;
		typedef typename TProtocol::resolver TResolver;

		SessionPool(TOwner* owner, _ASIO io_service& io) :
			ios(io),
			count(0),
			sessionPool(_STD move(TMemoryPool::Create(alloc))),
			owner_(owner),
			resolver(io),
			sessionStorage(TStorage::Create())
		{
			//corePtr->GetTrace()->DebugPoint(LogMsg::NewSessionPool);
			isDestruct = false;
		}

		~SessionPool()
		{
			//corePtr->GetTrace()->DebugPoint(LogMsg::FreeSessionPool);
			isDestruct = true;
			if (count > 0)
			{
				CloseAll();
			}
			assert(count == 0);
		}
		// 多线程同时new多次有一定概率出现线程争用
		shared_ptr<TSession> __fastcall NewSession()
		{
			//corePtr->GetTrace()->DebugPoint(LogMsg::NewSession);
			++count;
			// FIX: 看有什么更快的方案可以换
			// 构建session
			auto result = TMemoryPool::Malloc(sessionPool, this, &ios);

			// 加快返回速度，析构的erase执行时间绝对会在insert执行之后			
			ios.post([&, result]() {result->insertPosition = TStorage::Insert(sessionStorage, result); });
			return shared_ptr<TSession>(result, [this](TSession* ptr) {Free(ptr); });
		}

		unsigned int GetSessionCount() const
		{
			return count;
		}
		// 可在运行时和停止前调用
		void CloseAll()
		{
			// 只关掉连接，删除对象由智能指针处理			
			//corePtr->GetTrace()->DebugPoint(LogMsg::CloseAllSocket);
			if (isDestruct)
			{
				AsyncCloseAll();
				return;
			}
			ios.post(_BOOST bind(&SessionPool::AsyncCloseAll, this));
		}
		TOwner* GetOwner()
		{
			return owner_;
		}
		TResolver& GetResolver()
		{
			return resolver;
		}
		void __PreDestruct()
		{
			isDestruct = true;
		}
	private:
		TOwner* owner_;
		//_ASIO strand strand;
		// 表示是否处于析构状态
		bool isDestruct;

		//TCore* corePtr;
		_ASIO io_service& ios;
		_BOOST atomic<unsigned int> count;

		TResolver resolver;
		_BOOST system::error_code ignore;

		_STD allocator<TSession> alloc;
		// FIX:池(在freelock里找到的，是否好用待测试)
		TPoolPtr sessionPool;
		// 用作分配对象的指针存储

		TStoragePtr sessionStorage;

		void AsyncCloseAll()
		{
			for each (auto& var in *sessionStorage)
			{
				var->close(ignore);
			}
		}

		void FreeSession(TSession* ptr)
		{
			//corePtr->GetTrace()->DebugPoint(LogMsg::FreeSession);

			// 析构时不erase减少析构时间
			if (!isDestruct)
			{
				TStorage::Remove(sessionStorage, ptr->insertPosition);
			}

			// FIX: cpu
			// 在这种情况下使用lockfree可能会拖慢速度
			TMemoryPool::Free(sessionPool, ptr);
			--count;
		}

		void Free(TSession* ptr)
		{
			if (isDestruct)
			{
				// ioservice会在free之前停止，所以不能post
				FreeSession(ptr);
				return;
			}
			// 不能dispatch，此时插入位置不一定已经设置
			ios.post([this, ptr]() {FreeSession(ptr); });
		}
	};

}  // namespace FeedTheDog