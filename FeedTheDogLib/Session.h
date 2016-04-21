#pragma once
#include "SessionBase.h"
#include "Owner.h"
namespace FeedTheDog
{

	// FIX: 这样设计加密模块没办法添加,
	// 需要在send前经过加密函数,函数导出的话要写太多了
	template<typename TProtocol, typename TSessionPool>
	class SessionImpl :
		public SessionBase,
		protected Owner<TSessionPool>
	{
	public:
		typedef TProtocol TProtocol;
		typedef SessionImpl TSession;
		typedef typename TProtocol::socket TSocket;

		SessionImpl(TSessionPool* pool, io_service* ios) :
			socket_(*ios),
			Owner(pool)
		{
		}

		~SessionImpl()
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
		typedef typename TSessionPool::TStorageIterator TStorageIterator;
		TStorageIterator insertPosition;

	};
	template<typename TProtocol, typename TSessionPool>
	class Session;

	template<typename TSessionPool>
	class Session<_ASIO ip::tcp, TSessionPool> :
		public SessionImpl<_ASIO ip::tcp, TSessionPool>
	{
	public:
		friend TSessionPool;
		Session(TSessionPool* pool, io_service* ios) :
			SessionImpl(pool, ios)
		{

		}
		// Tcp
		template <typename MutableBufferSequence, typename ReadHandler>
		inline void AsyncReadSome(const MutableBufferSequence& buffers, ReadHandler&& handler)
		{
			socket_.async_read_some(buffers, _STD forward<ReadHandler>(handler));
		}
		// Tcp
		template <typename MutableBufferSequence, typename WriteHandler>
		inline void AsyncWriteSome(const MutableBufferSequence& buffers,
			WriteHandler&& handler)
		{
			socket_.async_write_some(buffers, _STD forward<WriteHandler>(handler));
		}


		// Tcp 的一些函数
		template <typename MutableBufferSequence, typename ReadHandler>
		inline void AsyncRead(const MutableBufferSequence& buffers, ReadHandler&& handler)
		{
			_ASIO async_read(socket_, buffers, _STD forward<ReadHandler>(handler));
		}



		template <typename MutableBufferSequence, typename WriteHandler>
		inline void AsyncWrite(const MutableBufferSequence& buffers, WriteHandler&& handler)
		{
			_ASIO async_write(socket_, buffers, _STD forward<WriteHandler>(handler));
		}

		template <typename MutableBufferSequence>
		inline _STD size_t Read(const MutableBufferSequence& buffers, _BOOST system::error_code& ec)
		{
			return _ASIO read(socket_, buffers, ec);
		}

		template <typename ConstBufferSequence>
		inline _STD size_t Write(const ConstBufferSequence& buffers, _BOOST system::error_code& ec)
		{
			return _ASIO write(socket_, buffers, ec);
		}

		template <typename Iterator, typename ComposedConnectHandler>
		inline void AsyncConnect(Iterator begin, ComposedConnectHandler&& handler)
		{
			_ASIO async_connect(socket_, begin, _STD forward<ComposedConnectHandler>(handler));
		}

		template <typename ConnectHandler>
		inline void AsyncConnect(
			const typename TProtocol::endpoint& peer_endpoint,
			ConnectHandler&& handler)
		{
			socket_.async_connect(peer_endpoint, _STD forward<ConnectHandler>(handler));
		}
	private:
	};
	template<typename TSessionPool>
	class Session<_ASIO ip::udp, TSessionPool> :
		public SessionImpl<_ASIO ip::udp, TSessionPool>
	{
	public:
		friend TSessionPool;
		Session(TSessionPool* pool, io_service* ios) :
			SessionImpl(pool, ios)
		{

		}
		// Udp
		template <typename MutableBufferSequence, typename ReadHandler>
		inline void AsyncReadSome(
			typename TProtocol::endpoint& endpoint,
			const MutableBufferSequence& buffers,
			ReadHandler&& handler)
		{
			socket_.async_receive_from(buffers, endpoint, _STD forward<ReadHandler>(handler));
		}

		// Udp
		template <typename MutableBufferSequence, typename WriteHandler>
		inline void AsyncWriteSome(
			typename TProtocol::endpoint& endpoint,
			const MutableBufferSequence& buffers,
			WriteHandler&& handler)
		{
			socket_.async_send_to(buffers, endpoint, _STD forward<WriteHandler>(handler));
		}
		// TODO: 用到再加
	private:
	};
}  // namespace FeedTheDog

