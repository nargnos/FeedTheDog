#pragma once
#include "Trait.h"
namespace FeedTheDog
{
	template<typename TProtocol, typename TSessionPool>
	class Session;
	class Worker;


	struct MemoryPoolStrategy
	{
		enum { PoolDefaultSize = 1024 };
		template<typename TSession>
		struct MemoryPool
		{
			typedef _BOOST lockfree::detail::freelist_stack<TSession> TPoolType;
			typedef unique_ptr<TPoolType> TPoolPtr;
			template<typename TOwner>
			static TSession* Malloc(TPoolPtr& pool, TOwner* owner_, io_service* ios)
			{
				return pool->construct<true, false>(owner_, ios);
			}
			static void Free(TPoolPtr& pool, TSession* ptr)
			{
				pool->destruct<true>(ptr);
			}
			static TPoolPtr Create(_STD allocator<TSession>& alloc)
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
			static TStorageIterator Insert(TStoragePtr& storage, TSession* insert)
			{
				return storage->insert(storage->end(), insert);
			}
			static void Remove(TStoragePtr& storage, TStorageIterator& pos)
			{
				storage->erase(pos);
			}
			static TStoragePtr Create()
			{
				return make_unique<TStorageType>();
			}
		};

	};
	struct SessionPoolTrait
	{
		typedef _ASIO ip::tcp TTcp;
		typedef _ASIO ip::udp TUdp;
		//typedef Worker TWorker;
		template<typename TProtocol>
		struct TSession
		{
			typedef Session<TProtocol, SessionPool<TProtocol,Worker, SessionPoolTrait, MemoryPoolStrategy, SessionStorageStrategy>> type;
		};

		/*template<typename TProtocol>
		struct TQueue
		{
			typedef _BOOST lockfree::queue<typename TSession<TProtocol>::type*> type;
		};*/
		typedef TSession<TTcp>::type TTcpSession;
		typedef TSession<TUdp>::type TUdpSession;


	};
	typedef SessionPoolTrait::TTcpSession TTcpSession;
	typedef SessionPoolTrait::TUdpSession TUdpSession;

}  // namespace FeedTheDog
