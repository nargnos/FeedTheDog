#pragma once
#include "Trait.h"
#include "SessionPoolTrait.h"
namespace FeedTheDog
{

	struct WorkerTrait
	{
		//typedef IService TService;

		typedef _ASIO ip::tcp TTcp;
		typedef _ASIO ip::udp TUdp;

		template<typename TProtocol, typename TOwner>
		struct TSessionPool :
			public SessionPoolTrait::TSessionPool<TProtocol, TOwner, MemoryPoolStrategy, SessionStorageStrategy>
		{
		};
	};
}  // namespace FeedTheDog
