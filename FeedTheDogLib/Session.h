#pragma once
#include "SessionBase.h"
#include "SessionPoolTrait.h"
namespace FeedTheDog
{

	template<typename TProtocol>
	class Session :
		public _STD enable_shared_from_this<Session<TProtocol>>,
		public SessionBase
	{
	public:
		typedef TProtocol TProtocol;
		typedef Session<TProtocol> TSession;
		typedef typename TProtocol::socket TSocket;
		typedef typename CoreTrait::TSessionPool<TProtocol>::type TSessionPool;
		friend TSessionPool;
		Session(TWorker* worker) :
			socket_(worker->GetIoService()),
			TSessionBase(worker)
		{
		}
		TSocket& GetSocket()
		{
			return socket_;
		}
		void Close()
		{
			if (socket_.is_open())
			{
				//socket_.shutdown(_ASIO socket_base::shutdown_type::shutdown_both);
				socket_.close(errorCode);
			}

		}
		~Session()
		{
			Close();
		}

	protected:
		TSocket socket_;
		// 当前在map中的位置
		typedef typename SessionPoolTrait::TSessionMultiMap<TProtocol>::type::iterator TMapIterator;
		TMapIterator mapPosition;

	};


}  // namespace FeedTheDog

