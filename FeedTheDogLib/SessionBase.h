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

		virtual ~SessionBase() = default;

		_ASIO io_service& GetIoService()
		{
			return socket_.get_io_service();
		}
		TSocket& GetSocket()
		{
			return socket_;
		}
		const TSocket& GetSocket() const
		{
			return socket_;
		}
		void Close(_BOOST system::error_code& ec)
		{
			socket_.close(ec);
		}
		void ShutDown(typename TSocket::shutdown_type what, _BOOST system::error_code& ec)
		{
			socket_.shutdown(what, ec);
		}
		bool IsOpen() const
		{
			return socket_.is_open();
		}

	protected:
		TSocket socket_;
	};


}  // namespace FeedTheDog

