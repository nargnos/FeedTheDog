#pragma once
#include "SessionImpl.h"
namespace FeedTheDog
{
	template<bool hasBuffer>
	class SessionBufferType;

	template<>
	class SessionBufferType<true>
	{
	public:
		enum
		{
			BufferSize = 1024 * 8
		};
		using TBufferType = _STD array<unsigned char, BufferSize>;

		TBufferType& GetBuffer()
		{
			return buffer_;
		}
		const TBufferType& GetBuffer() const
		{
			return buffer_;
		}
	protected:
		TBufferType buffer_;
	};
	template<>
	class SessionBufferType<false>
	{
	};

	template<typename TProtocol, bool hasBuffer>
	class Session :
		public SessionImpl<TProtocol>,
		public SessionBufferType<hasBuffer>
	{
	public:
		Session(io_service& ios) :
			SessionImpl(ios)
		{
		}
	};
}  // namespace FeedTheDog

