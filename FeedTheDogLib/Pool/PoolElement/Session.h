#pragma once
#include "..\..\Common.h"
#include "SessionBase.h"
namespace FeedTheDog
{

	template<typename TProtocol>
	class Session:
		public _STD enable_shared_from_this<Session<TProtocol>>,
		public SessionBase
	{
	public:
		typedef Session<TProtocol> TSession;
		typedef typename TProtocol::socket TSocket;
		
		Session(TSessionPool* ptr,_ASIO io_service& io):
			socket_(io),
			TSessionBase(ptr)
		{
		}
		TSocket& GetSocket()
		{
			return socket_;
		}
	protected:
		TSocket socket_;
	};


}  // namespace FeedTheDog

