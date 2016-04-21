#pragma once
#include "Session.h"
#include "Owner.h"
namespace FeedTheDog
{
	class SpinLock :
		_BOOST noncopyable
	{
	public:
		SpinLock()
		{
		}
		inline bool TryLock()
		{
			return !lock.test_and_set(_STD memory_order_acquire);
		}
		inline void Lock()
		{
			unsigned int i = 0;
			while (!TryLock())
			{
				ThreadYield(++i);
			}
		}
		inline void Unlock()
		{
			lock.clear(_STD memory_order_release);
		}
	private:
		inline void ThreadYield(unsigned int k)
		{
			if (k < 4)
			{
			}
#if defined( BOOST_SMT_PAUSE )
			else if (k < 16)
			{
				BOOST_SMT_PAUSE
			}
#endif
			else
			{
				_STD this_thread::yield();
			}
		}
		_STD atomic_flag lock = ATOMIC_VAR_INIT(false);
	};
	class SpinLockGuard :
		public _BOOST noncopyable
	{
	public:
		SpinLockGuard(SpinLock& lock) :
			lock_(lock)
		{
			lock_.Lock();
		}

		~SpinLockGuard()
		{
			lock_.Unlock();
		}

	private:
		SpinLock& lock_;
	};

	template<typename TProtocol,
		typename TOwner,
		typename TSessionPoolPolicy,
		typename TMemoryPoolPolicy,
		typename TSessionStoragePolicy>
	class SessionPool :
		public _BOOST noncopyable,
		public Owner<TOwner>
	{
	public:
		typedef typename TSessionPoolPolicy::template TSession<TProtocol, SessionPool>::TSessionType TSession;

		typedef typename TMemoryPoolPolicy::template MemoryPool<TSession> TMemoryPoolPolicy;
		typedef typename TMemoryPoolPolicy::TPoolPtr TPoolPtr;

		typedef typename TSessionStoragePolicy::template SessionStorage<TSession> TStorage;
		typedef typename TStorage::TStoragePtr TStoragePtr;

		typedef typename TStorage::TStorageIterator TStorageIterator;
		typedef typename TStorage::TStorageValue TStorageValue;

		typedef typename TProtocol::socket TSocket;
		typedef typename TProtocol::resolver TResolver;

		SessionPool(TOwner* owner, _ASIO io_service& io) :
			ios(io),
			count(0),
			sessionPool(_STD move(TMemoryPoolPolicy::Create(alloc))),
			resolver(io),
			sessionStorage(TStorage::Create()),
			Owner(owner)
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
		// ���߳�ͬʱnew�����һ�����ʳ����߳�����
		shared_ptr<TSession> __fastcall NewSession()
		{
			count.fetch_add(1, _BOOST memory_order_relaxed);
			// FIX: ����ʲô����ķ������Ի�
			// ����session
			auto result = TMemoryPoolPolicy::Malloc(sessionPool, this, &ios);

			{
				SpinLockGuard guard(lock);
				result->insertPosition = TStorage::Insert(sessionStorage, result);
			}
			return shared_ptr<TSession>(result, [this](TSession* ptr) {FreeSession(ptr); });
		}

		inline unsigned int GetSessionCount() const
		{
			// count�����ü�����ʹ�ö�������νִ��˳���
			return count.load(_BOOST memory_order_relaxed);
		}
		// ��������ʱ��ֹͣǰ����
		void CloseAll()
		{
			// ֻ�ص����ӣ�ɾ������������ָ�봦��
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
	private:
		TResolver resolver;
		_STD allocator<TSession> alloc;
		// FIX:��(��freelock���ҵ��ģ��Ƿ���ô�����)
		TPoolPtr sessionPool;
		// ������������ָ��洢
		TStoragePtr sessionStorage;
		_BOOST atomic<unsigned int> count;
		_ASIO io_service& ios;

		SpinLock lock;
		// ��ʾ�Ƿ�������״̬
		bool isDestruct;
		void AsyncCloseAll()
		{
			_BOOST system::error_code ignore;
			for each (auto& var in *sessionStorage)
			{
				var->GetSocket().close(ignore);
			}

		}
		void FreeSession(TSession* ptr)
		{
			auto& pos = ptr->insertPosition;
			if (!isDestruct)
			{
				SpinLockGuard guard(lock);
				TStorage::Remove(sessionStorage, pos);
			}
			count.fetch_sub(1, _BOOST memory_order_relaxed);
			TMemoryPoolPolicy::Free(sessionPool, ptr);
		}

	};

}  // namespace FeedTheDog