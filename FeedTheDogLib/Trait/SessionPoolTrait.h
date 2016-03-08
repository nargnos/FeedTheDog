#pragma once
#include "..\Common.h"
#include "Trait.h"
#include "..\Pool\PoolElement\Session.h"
namespace FeedTheDog
{

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
		struct TPoolType
		{
			typedef _BOOST lockfree::detail::freelist_stack<typename TSession<TProtocol>::type> type;
		};
		typedef TSession<TTcp>::type TTcpSession;
		typedef TSession<TUdp>::type TUdpSession;
	};
	typedef SessionPoolTrait::TTcpSession TTcpSession;
	typedef SessionPoolTrait::TUdpSession TUdpSession;

}  // namespace FeedTheDog
