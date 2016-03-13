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
		typedef Session<TProtocol> TSession;
		typedef typename TProtocol::socket TSocket;
		friend typename CoreTrait::TSessionPool;
		Session(TCore* corePtr, _ASIO io_service& io):
			socket_(io),
			TSessionBase(corePtr,io)
		{
#ifdef _DEBUG
			core->GetTrace()->TracePoint(LogMsg::NewSession);
#endif // _DEBUG
		}
		TSocket& GetSocket()
		{
			return socket_;
		}
		~Session()
		{
#ifdef _DEBUG
			core->GetTrace()->TracePoint(LogMsg::FreeSession);
#endif // _DEBUG
		}
	protected:
		TSocket socket_;
		// 当前在map中的位置
		typedef typename SessionPoolTrait::TSessionSave<TProtocol>::SessionSave TSessionSave;
		TSessionSave mapPosition;		
	};


}  // namespace FeedTheDog

