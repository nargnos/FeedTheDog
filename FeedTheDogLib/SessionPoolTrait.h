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
		template<typename TProtocol>
		struct TPool
		{
			
			typedef _BOOST lockfree::detail::freelist_stack<typename TSession<TProtocol>::type> type;
		};
		/*template<typename TProtocol>
		struct TQueue
		{
			typedef _BOOST lockfree::queue<typename TSession<TProtocol>::type*> type;
		};*/
		typedef TSession<TTcp>::type TTcpSession;
		typedef TSession<TUdp>::type TUdpSession;
		template<typename TProtocol>
		struct TSessionMultiMap
		{
			typedef _STD unordered_multimap<const char*, typename SessionPoolTrait::TSession<TProtocol>::type*> type;
		};
		
	};
	typedef SessionPoolTrait::TTcpSession TTcpSession;
	typedef SessionPoolTrait::TUdpSession TUdpSession;

}  // namespace FeedTheDog
