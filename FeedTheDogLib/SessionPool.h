#pragma once
#include "SpinLock.h"
namespace FeedTheDog
{

	template<typename TProtocol,
		typename TSessionPoolPolicy,
		typename TMemoryPoolPolicy,
		typename TSessionStoragePolicy>
	class SessionPool :
		public _BOOST noncopyable
	{
	public:
		typedef typename TProtocol::socket TSocket;
		typedef typename TProtocol::resolver TResolver;

#define TDEFINES \
typedef typename TSessionStoragePolicy::template SessionStorage<TSessionType> TStorage;\
		typedef typename TStorage::TStoragePtr TStoragePtr;\
		typedef typename TStorage::TStorageIterator TStorageIterator;\
		typedef typename TStorage::TStorageValue TStorageValue;\
		typedef typename TMemoryPoolPolicy::template MemoryPool<TSessionType> TMemoryPool;\
		typedef typename TMemoryPool::TPoolType TMemoryPoolType;\
		typedef typename TMemoryPool::TPoolPtr TMemoryPoolPtr;


		template<bool hasBuffer>
		struct THasBuffer
		{
			typedef typename TSessionPoolPolicy::template TSession<TProtocol, SessionPool, hasBuffer >::TSessionType TSessionType;
			TDEFINES;
		};

	
		SessionPool(_ASIO io_service& io) :
			ios(io),
			count(0),
			resolver(io),
			sessionPool(_STD move(TMemoryPool::Create(alloc))),
			sessionPoolNoBuffer(_STD move(TMemoryPool_NoBuffer::Create(allocNoBuffer))),
			sessionStorage(TStorage::Create()),
			sessionStorageNoBuffer(TStorage_NoBuffer::Create())
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
			assert(count == 0);
		}
		// 多线程同时new多次有一定概率出现线程争用
		// FIX: 这样有一些代码冗余
		template<bool hasBuffer = true>
		shared_ptr<typename THasBuffer<hasBuffer>::TSessionType> __fastcall NewSession();

		template<>
		shared_ptr<typename THasBuffer<true>::TSessionType> __fastcall NewSession<true>()
		{
			count.fetch_add(1, _BOOST memory_order_relaxed);
			auto result = TMemoryPool::Malloc(sessionPool, this, ios);;
			{
				SpinLockGuard guard(lock);
				result->insertPosition = TStorage::Insert(sessionStorage, result);
			}
			return shared_ptr<TSession>(result, [this](TSession* ptr)
			{
				FreeSession<true>(ptr);
			});
		}
		template<>
		shared_ptr<typename THasBuffer<false>::TSessionType> __fastcall NewSession<false>()
		{
			count.fetch_add(1, _BOOST memory_order_relaxed);
			auto result = TMemoryPool_NoBuffer::Malloc(sessionPoolNoBuffer, this, ios);;
			{
				SpinLockGuard guard(lockNoBuffer);
				result->insertPosition = TStorage_NoBuffer::Insert(sessionStorageNoBuffer, result);
			}
			return shared_ptr<TSession_NoBuffer>(result, [this](TSession_NoBuffer* ptr)
			{
				FreeSession<false>(ptr);
			});
		}

		inline unsigned int GetSessionCount() const
		{
			// count的设置及其结果使用都是无所谓执行顺序的
			return count.load(_BOOST memory_order_relaxed);
		}
		// 可在运行时和停止前调用
		void CloseAll()
		{
			// 只关掉连接，删除对象由智能指针处理
			if (isDestruct)
			{
				AsyncCloseAll();
				return;
			}
			ios.post(_BOOST bind(&SessionPool::AsyncCloseAll, this));
		}

		TResolver& GetResolver()
		{
			return resolver;
		}
		void __PreDestruct()
		{
			isDestruct = true;
		}

		typedef typename THasBuffer<true> _THasBuffer;
		typedef typename _THasBuffer::TSessionType TSession;

		typedef typename THasBuffer<false> _TNoBuffer;
		typedef typename _TNoBuffer::TSessionType TSession_NoBuffer;
	private:
		// 不想让两种session共用存储位置和锁
		typedef typename _THasBuffer::TStorage TStorage;
		typedef typename _THasBuffer::TStoragePtr TStoragePtr;
		typedef typename _THasBuffer::TMemoryPool TMemoryPool;
		typedef typename _THasBuffer::TMemoryPoolPtr TMemoryPoolPtr;

		_STD allocator<TSession> alloc;
		TMemoryPoolPtr sessionPool;
		TStoragePtr sessionStorage;
		SpinLock lock;

		typedef typename _TNoBuffer::TStorage TStorage_NoBuffer;
		typedef typename _TNoBuffer::TStoragePtr TStoragePtr_NoBuffer;
		typedef typename _TNoBuffer::TMemoryPool TMemoryPool_NoBuffer;
		typedef typename _TNoBuffer::TMemoryPoolPtr TMemoryPoolPtr_NoBuffer;

		_STD allocator<TSession_NoBuffer> allocNoBuffer;
		TMemoryPoolPtr_NoBuffer sessionPoolNoBuffer;
		TStoragePtr_NoBuffer sessionStorageNoBuffer;
		SpinLock lockNoBuffer;

		TResolver resolver;
		_BOOST atomic<unsigned int> count;
		_ASIO io_service& ios;

		// 表示是否处于析构状态
		bool isDestruct;
		void AsyncCloseAll()
		{
			_BOOST system::error_code ignore;
			for each (auto& var in *sessionStorage)
			{
				var->Close(ignore);
			}
			for each (auto& var in *sessionStorageNoBuffer)
			{
				var->Close(ignore);
			}
		}

		template<bool hasBuffer = true>
		void FreeSession(typename THasBuffer<hasBuffer>::TSessionType* ptr);

		template<>
		void FreeSession<true>(typename THasBuffer<true>::TSessionType* ptr)
		{
			auto& pos = ptr->insertPosition;
			if (!isDestruct)
			{
				SpinLockGuard guard(lock);
				TStorage::Remove(sessionStorage, pos);
			}
			count.fetch_sub(1, _BOOST memory_order_relaxed);
			TMemoryPool::Free(sessionPool, ptr);
		}
		template<>
		void FreeSession<false>(typename THasBuffer<false>::TSessionType* ptr)
		{
			auto& pos = ptr->insertPosition;
			if (!isDestruct)
			{
				SpinLockGuard guard(lockNoBuffer);
				TStorage_NoBuffer::Remove(sessionStorageNoBuffer, pos);
			}
			count.fetch_sub(1, _BOOST memory_order_relaxed);
			TMemoryPool_NoBuffer::Free(sessionPoolNoBuffer, ptr);
		}
	};

}  // namespace FeedTheDog