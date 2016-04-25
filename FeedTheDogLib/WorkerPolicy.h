#pragma once
#include "Define.h"
#include "SessionPoolPolicy.h"
namespace FeedTheDog
{
	struct WorkerPolicy
	{
		typedef _ASIO ip::tcp TTcp;
		typedef _ASIO ip::udp TUdp;

		template<typename TProtocol, typename TOwner>
		struct TSessionPool
		{
			typedef SessionPool<TProtocol, TOwner, SessionPoolPolicy, MemoryPoolPolicy, SessionStoragePolicy> TSessionPoolType;
		};
	};
}  // namespace FeedTheDog
