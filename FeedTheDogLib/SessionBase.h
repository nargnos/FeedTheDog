#pragma once
namespace FeedTheDog
{
	template<typename TProtocol>
	class SessionBase :
		public _BOOST noncopyable
	{
	public:
		typedef TProtocol TProtocol;
		typedef typename TProtocol::socket TSocket;

		SessionBase(io_service& ios) :
			socket_(ios)
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

	protected:
		TSocket socket_;

	};
	
	
}  // namespace FeedTheDog

