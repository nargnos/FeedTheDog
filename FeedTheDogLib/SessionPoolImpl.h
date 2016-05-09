#pragma once
#include "SpinLock.h"
namespace FeedTheDog
{
	struct DlmallocAllocatorNewDelete
	{
		typedef _STD size_t size_type;
		typedef _STD ptrdiff_t difference_type;

		static char * malloc(const size_type bytes)
		{
			return reinterpret_cast<char*>(_BOOST container::dlmalloc_malloc(bytes));
		}
		static void free(char * const block)
		{
			_BOOST container::dlmalloc_free(block);
		}
	};
	template<typename TSession>
	class SessionPoolImpl :
		public _BOOST noncopyable
	{
	public:
		// using TSession = Session<_ASIO ip::tcp, true>;

		using TAlloctor = _BOOST container::allocator<TSession*>;
		using TStorage = _STD list<TSession*, TAlloctor>;
		using TStorageIterator = typename TStorage::iterator;
		using TPool = _BOOST pool<DlmallocAllocatorNewDelete>;

		SessionPoolImpl(_ASIO io_service& ios, size_t poolSize, _STD atomic<size_t>& counter) :
			sessionPool_(sizeof(TSession), poolSize),
			ios_(ios),
			counter_(counter),
			destory_(false)
		{
		}

		virtual ~SessionPoolImpl()
		{
			destory_ = true;

			// 析构所有session
			for each (auto& var in storage_)
			{
				var->~TSession();
				sessionPool_.free(var);
			}
		}
		// 有比较小的概率发生竞争
		shared_ptr<TSession> FASTCALL NewSession()
		{
			assert(!destory_);
			TSession* ptr = nullptr;
			IncCounter();

			// 生成对象
			{
				SpinLockGuard lockGuard(sessionPoolLock_);
				ptr = reinterpret_cast<TSession*>(sessionPool_.malloc());
			}
			if (ptr == nullptr)
			{
				throw OutOfMemory();
			}
			new (ptr) TSession(ios_);


			// ptr不使用时回收
			return shared_ptr<TSession>(ptr,
				[this, pos = InsertStorage(ptr)](TSession* ptr) mutable
			{
				if (destory_)
				{
					return;
				}

				DecCounter();
				ptr->~TSession();
				{
					SpinLockGuard storagelockGuard(storageLock_);
					storage_.erase(pos);
				}

				{
					SpinLockGuard poolLockGuard(sessionPoolLock_);
					sessionPool_.free(ptr);
				}
			});

		}

	protected:
		TStorageIterator FASTCALL InsertStorage(TSession* ptr)
		{
			SpinLockGuard lockGuard(storageLock_);
			storage_.push_front(ptr);
			return storage_.begin();
		}

		inline static _STD exception OutOfMemory()
		{
			return _STD exception("out of memory");
		}
		inline void IncCounter()
		{
			counter_.fetch_add((size_t)1, _STD memory_order_relaxed);
		}
		inline void DecCounter()
		{
			counter_.fetch_sub((size_t)1, _STD memory_order_relaxed);
		}

		ALIGN SpinLock storageLock_;
		ALIGN TStorage storage_;
		ALIGN SpinLock sessionPoolLock_;
		ALIGN TPool sessionPool_;
		_STD atomic<size_t>& counter_;
		_ASIO io_service& ios_;
		bool destory_;
	};
}  // namespace FeedTheDog
