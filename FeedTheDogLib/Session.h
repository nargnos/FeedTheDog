#pragma once
#include "SessionBase.h"
#include "Owner.h"
namespace FeedTheDog
{
	// FIX: 这样设计加密模块没办法添加,
	// 需要在send前经过加密函数,函数导出的话要写太多了
	template<typename TProtocol,typename TSessionPool>
	class Session :
		public _STD enable_shared_from_this<Session<TProtocol, TSessionPool>>,
		public SessionBase,
		public TProtocol::socket,
		protected Owner<TSessionPool>
	{
	public:
		typedef TProtocol TProtocol;
		typedef Session<TProtocol, TSessionPool> TSession;
		typedef typename TProtocol::socket TSocket;
		friend TSessionPool;
		Session(TSessionPool* pool,io_service* ios) :
			TSocket(*ios),
			Owner(pool)
		{
			//async_connect
			//async_read_some
			//async_write_some
			//read_some
			//_ASIO ip::tcp::socket::
		}
	
		~Session()
		{
			
		}
		TSessionPool* GetSessionPool()
		{
			return owner_;
		}
	protected:
		
		typedef typename TSessionPool::TStorageIterator TStorageIterator;
		TStorageIterator insertPosition;

	};


}  // namespace FeedTheDog

