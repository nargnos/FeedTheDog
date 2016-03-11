#pragma once
#include "Trait.h"
namespace FeedTheDog
{
	template<typename TProtocol>
	class Session;
	struct SessionPoolTrait
	{
		
		typedef _ASIO ip::tcp TTcp;
		typedef _ASIO ip::udp TUdp;		
		typedef Worker TWorker;
		template<typename TProtocol>
		struct TSession
		{
			typedef Session<TProtocol> type;
		};
		template<typename T>
		struct TPool
		{
			typedef _BOOST lockfree::detail::freelist_stack<T> type;
		};
		template<typename TProtocol>
		struct TPoolType
		{
			typedef typename TPool<typename TSession<TProtocol>::type>::type type;
		};
		typedef TSession<TTcp>::type TTcpSession;
		typedef TSession<TUdp>::type TUdpSession;
		template<typename TProtocol>
		struct TSessionSave
		{
			typedef concurrent_unordered_multimap<const char*, typename SessionPoolTrait::TSession<TProtocol>::type*> MapType;
			typedef typename MapType::iterator SessionSave;
			typedef typename MapType::value_type MapValue;
		};
		
	};
	typedef SessionPoolTrait::TTcpSession TTcpSession;
	typedef SessionPoolTrait::TUdpSession TUdpSession;

}  // namespace FeedTheDog
