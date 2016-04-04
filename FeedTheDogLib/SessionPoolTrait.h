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
		// FIX: 每个池初始的大小，这里可以改到配置文件里
		enum { PoolDefaultSize = 1024 };
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
		struct TSessionStorage
		{
			typedef typename SessionPoolTrait::TSession<TProtocol>::type TValue;
			typedef _STD list<TValue*> type;
		};
		
	};
	typedef SessionPoolTrait::TTcpSession TTcpSession;
	typedef SessionPoolTrait::TUdpSession TUdpSession;

}  // namespace FeedTheDog
