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
		
		Session(TCore* core) :
			TSessionBase(core),
			socket_(core->GetIoService())
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

