#pragma once
#include "Define.h"
namespace FeedTheDog
{
	template<typename TProtocol>
	class Session
	{
	public:
		using TSocket = typename TProtocol::socket;
		using TBuffer = _STD array<unsigned char, BufferSize>;

		explicit Session(_ASIO io_service& ios):
			socket_(ios)
		{
		}

		~Session()
		{
		}
		//void Encrypt()
		//{

		//}
		//void Decrypt()
		//{
		//	// Vector才能修改大小，这样还要弄一个加密版本的session
		//}
		_ASIO io_service& GetIoService()
		{
			return socket_.get_io_service();
		}
		TSocket& GetSocket()
		{
			return socket_;
		}
		TBuffer& GetBuffer()
		{
			return buffer_;
		}
	private:
		TSocket socket_;
		TBuffer buffer_;

	};
}  // namespace FeedTheDog
