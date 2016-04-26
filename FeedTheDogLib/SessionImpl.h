#pragma once
#include "SessionBase.h"
namespace FeedTheDog
{
	template<typename TProtocol>
	class SessionImpl;

	template<>
	class SessionImpl<_ASIO ip::tcp> :
		public SessionBase<_ASIO ip::tcp>
	{
	public:
		SessionImpl(io_service& ios) :
			SessionBase(ios)
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
	template<>
	class SessionImpl<_ASIO ip::udp> :
		public SessionBase<_ASIO ip::udp>
	{
	public:

		SessionImpl(io_service& ios) :
			SessionBase(ios)
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
