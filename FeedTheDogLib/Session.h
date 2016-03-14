#pragma once
#include "SessionBase.h"
#include "SessionPoolTrait.h"
namespace FeedTheDog
{

	template<typename TProtocol>
	class Session:
		public _STD enable_shared_from_this<Session<TProtocol>>,
		public SessionBase
	{
	public:
		typedef TProtocol TProtocol;
		typedef Session<TProtocol> TSession;
		typedef typename TProtocol::socket TSocket;
		typedef typename CoreTrait::TSessionPool<TProtocol>::type TSessionPool;
		friend TSessionPool;
		Session(TCore* corePtr, _ASIO io_service& io):
			socket_(io),
			TSessionBase(corePtr,io)
		{
		}
		TSocket& GetSocket()
		{
			return socket_;
		}
		~Session()
		{
		}
	protected:
		TSocket socket_;
		// 当前在map中的位置
		typedef typename SessionPoolTrait::TSessionMultiMap<TProtocol>::type::iterator TMapIterator;
		TMapIterator mapPosition;
	};


}  // namespace FeedTheDog

