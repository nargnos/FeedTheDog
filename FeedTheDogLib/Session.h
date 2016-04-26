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
		typedef _STD array<unsigned char, BufferSize> TBufferType;
		TBufferType& GetBuffer()
		{
			return buffer;
		}
	protected:
		TBufferType buffer;

	};
	template<>
	class SessionBufferType<false>
	{
	};

	template<typename TProtocol, typename TSessionPool, bool hasBuffer>
	class Session :
		public SessionImpl<TProtocol>,
		public SessionBufferType<hasBuffer>
	{
	public:
		friend TSessionPool;
		Session(io_service& ios) :
			SessionImpl(ios)
		{
		}
		virtual ~Session()
		{
		}
	protected:
		typedef typename TSessionPool::template THasBuffer<hasBuffer>::TStorageIterator TStorageIterator;
		TStorageIterator insertPosition;
	};
}  // namespace FeedTheDog

