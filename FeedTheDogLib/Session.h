#pragma once
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
		
		Session(const shared_ptr<TCore>& corePtr, _ASIO io_service& io):
			socket_(io),
			TSessionBase(corePtr,io)
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

