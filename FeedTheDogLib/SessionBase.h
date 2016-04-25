#pragma once
#include "Owner.h"
namespace FeedTheDog
{
	template<typename TProtocol, typename TSessionPool>
	class SessionBase :
		public _BOOST noncopyable,
		protected Owner<TSessionPool>
	{
	public:
		typedef TProtocol TProtocol;
		typedef typename TProtocol::socket TSocket;

		SessionBase(TSessionPool* pool, io_service* ios) :
			socket_(*ios),
			Owner(pool)
		{
		}

		virtual ~SessionBase()
		{
		}

		inline _ASIO io_service& GetIoService()
		{
			return socket_.get_io_service();
		}
		inline typename TProtocol::socket& GetSocket()
		{
			return socket_;
		}
		inline const typename TProtocol::socket& GetSocket() const
		{
			return socket_;
		}
		inline void Close(_BOOST system::error_code& ec)
		{
			socket_.close(ec);
		}
		inline void ShutDown(_ASIO socket_base::shutdown_type what, _BOOST system::error_code& ec)
		{
			socket_.shutdown(what, ec);
		}
		inline bool IsOpen() const
		{
			return socket_.is_open();
		}

		inline TSessionPool* GetSessionPool()
		{
			return owner_;
		}

	protected:
		TSocket socket_;

	};
	
	
}  // namespace FeedTheDog

