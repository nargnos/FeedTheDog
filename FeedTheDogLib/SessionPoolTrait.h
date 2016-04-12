#pragma once
#include "Trait.h"
namespace FeedTheDog
{

	struct MemoryPoolStrategy
	{
		enum { PoolDefaultSize = 1024 };
		template<typename TSession>
		struct MemoryPool
		{
			typedef _BOOST lockfree::detail::freelist_stack<TSession> TPoolType;
			typedef unique_ptr<TPoolType> TPoolPtr;
			template<typename TOwner>
			inline static TSession* Malloc(TPoolPtr& pool, TOwner* owner_, io_service* ios)
			{
				return pool->construct<true, false>(owner_, ios);
			}
			inline static void Free(TPoolPtr& pool, TSession* ptr)
			{
				pool->destruct<true>(ptr);
			}
			inline static TPoolPtr Create(_STD allocator<TSession>& alloc)
			{
				return make_unique<TPoolType>(alloc, PoolDefaultSize);
			}
		};

	};

	struct SessionStorageStrategy
	{
		template<typename TSession>
		struct SessionStorage
		{
			typedef _STD list<TSession*> TStorageType;
			typedef unique_ptr<TStorageType> TStoragePtr;
			typedef typename TStorageType::value_type TStorageValue;
			typedef typename TStorageType::iterator TStorageIterator;
			inline static TStorageIterator Insert(TStoragePtr& storage, TSession* insert)
			{
				return storage->insert(storage->end(), insert);
			}
			inline static void Remove(TStoragePtr& storage, TStorageIterator& pos)
			{
				storage->erase(pos);
			}
			inline static TStoragePtr Create()
			{
				return make_unique<TStorageType>();
			}
		};

	};
	struct SessionPoolTrait
	{
		template<typename TProtocol,
			typename TOwner,
			typename TMemoryPool,
			typename TSessionStorage>
		struct TSessionPool
		{
			typedef SessionPool<TProtocol, TOwner, SessionPoolTrait, TMemoryPool, TSessionStorage> TSessionPoolType;
			typedef Session<TProtocol, TSessionPoolType> TSessionType;
		};

	};
}  // namespace FeedTheDog
