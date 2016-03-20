#pragma once
#include "SessionBase.h"
#include "SessionPoolTrait.h"
namespace FeedTheDog
{

	template<typename TProtocol>
	class Session :
		public _STD enable_shared_from_this<Session<TProtocol>>,
		public SessionBase,
		public TProtocol::socket
	{
	public:
		typedef TProtocol TProtocol;
		typedef Session<TProtocol> TSession;
		typedef typename TProtocol::socket TSocket;
		typedef typename CoreTrait::TSessionPool<TProtocol>::type TSessionPool;
		friend TSessionPool;
		Session(TWorker* worker) :
			TSocket(worker->GetIoService()),
			TSessionBase(worker)
		{
		}
	
		~Session()
		{
			
		}

	protected:
		
		// 当前在map中的位置
		typedef typename SessionPoolTrait::TSessionMultiMap<TProtocol>::type::iterator TMapIterator;
		TMapIterator mapPosition;

	};


}  // namespace FeedTheDog

