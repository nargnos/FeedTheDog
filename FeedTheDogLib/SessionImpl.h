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
			SessionBase(ios) {}
		// Tcp
		template <typename TMutableBufferSequence, typename TReadHandler>
		void FASTCALL AsyncReadSome(const TMutableBufferSequence& buffers, TReadHandler&& handler)
		{
			socket_.async_read_some(buffers, _STD forward<TReadHandler>(handler));
		}
		// Tcp
		template <typename TMutableBufferSequence, typename TWriteHandler>
		void FASTCALL AsyncWriteSome(const TMutableBufferSequence& buffers,
			TWriteHandler&& handler)
		{
			socket_.async_write_some(buffers, _STD forward<TWriteHandler>(handler));
		}


		// Tcp 的一些函数
		template <typename TMutableBufferSequence, typename TReadHandler>
		void FASTCALL AsyncRead(const TMutableBufferSequence& buffers, TReadHandler&& handler)
		{
			_ASIO async_read(socket_, buffers, _STD forward<TReadHandler>(handler));
		}

		template <typename TMutableBufferSequence, typename TWriteHandler>
		void FASTCALL AsyncWrite(const TMutableBufferSequence& buffers, TWriteHandler&& handler)
		{
			_ASIO async_write(socket_, buffers, _STD forward<TWriteHandler>(handler));
		}

		template <typename TMutableBufferSequence>
		_STD size_t FASTCALL Read(const TMutableBufferSequence& buffers, _BOOST system::error_code& ec)
		{
			return _ASIO read(socket_, buffers, ec);
		}

		template <typename TConstBufferSequence>
		_STD size_t FASTCALL Write(const TConstBufferSequence& buffers, _BOOST system::error_code& ec)
		{
			return _ASIO write(socket_, buffers, ec);
		}

		template <typename TIterator, typename TComposedConnectHandler>
		void FASTCALL AsyncConnect(TIterator begin, TComposedConnectHandler&& handler)
		{
			_ASIO async_connect(socket_, begin, _STD forward<TComposedConnectHandler>(handler));
		}

		template <typename TConnectHandler>
		void FASTCALL AsyncConnect(
			const typename TProtocol::endpoint& peer_endpoint,
			TConnectHandler&& handler)
		{
			socket_.async_connect(peer_endpoint, _STD forward<TConnectHandler>(handler));
		}
	};

	template<>
	class SessionImpl<_ASIO ip::udp> :
		public SessionBase<_ASIO ip::udp>
	{
	public:

		SessionImpl(io_service& ios) :
			SessionBase(ios) {}

		// Udp
		template <typename TMutableBufferSequence, typename TReadHandler>
		void FASTCALL AsyncReadSome(
			typename TProtocol::endpoint& endpoint,
			const TMutableBufferSequence& buffers,
			TReadHandler&& handler)
		{
			socket_.async_receive_from(buffers, endpoint, _STD forward<TReadHandler>(handler));
		}

		// Udp
		template <typename TMutableBufferSequence, typename TWriteHandler>
		void FASTCALL AsyncWriteSome(
			typename TProtocol::endpoint& endpoint,
			const TMutableBufferSequence& buffers,
			TWriteHandler&& handler)
		{
			socket_.async_send_to(buffers, endpoint, _STD forward<TWriteHandler>(handler));
		}
		// TODO: 用到再加
	};
}  // namespace FeedTheDog
