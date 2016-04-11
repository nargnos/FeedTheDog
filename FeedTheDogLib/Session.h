#pragma once
#include "SessionBase.h"
namespace FeedTheDog
{

	template<typename TProtocol,typename TSessionPool>
	class Session :
		public _STD enable_shared_from_this<Session<TProtocol, TSessionPool>>,
		public SessionBase<TSessionPool>,
		public TProtocol::socket
	{
	public:
		typedef TProtocol TProtocol;
		typedef Session<TProtocol, TSessionPool> TSession;
		typedef typename TProtocol::socket TSocket;
		friend TSessionPool;
		Session(TSessionPool* pool,io_service* ios) :
			TSocket(*ios),
			TSessionBase(pool)
		{
		}
	
		~Session()
		{
			
		}

	protected:
		
		typedef typename TSessionPool::TStorageIterator TStorageIterator;
		TStorageIterator insertPosition;

	};


}  // namespace FeedTheDog

