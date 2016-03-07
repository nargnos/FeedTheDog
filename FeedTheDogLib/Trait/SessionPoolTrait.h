#pragma once
#include "..\Common.h"
#include "Trait.h"
#include "..\Pool\PoolElement\Session.h"
namespace FeedTheDog
{

	struct SessionPoolTrait
	{
		typedef Session<_ASIO ip::tcp> TTcpSession;
		typedef Session<_ASIO ip::udp> TUdpSession;
		
	};
	typedef SessionPoolTrait::TTcpSession TTcpSession;
	typedef SessionPoolTrait::TUdpSession TUdpSession;

}  // namespace FeedTheDog
