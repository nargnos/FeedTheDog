#pragma once
namespace FeedTheDog
{
	class _SessionBase :
		public _BOOST noncopyable
	{
	public:
		static const unsigned int BufferSize = 1024 * 6;
		typedef _STD array<unsigned char, BufferSize> TBufferType;
		_SessionBase()
		{
		}
		virtual ~_SessionBase()
		{
		}
		TBufferType& GetBuffer()
		{
			return buffer;
		}
	protected:
		TBufferType buffer;
	};

	template<typename TSessionPool>
	class SessionBase :
		public _SessionBase
	{
	public:
		typedef SessionBase TSessionBase;
	
		SessionBase(TSessionPool* pool) :
			sessionPool(pool)
		{
		}
		TSessionPool * SessionBase::GetSessionPool()
		{
			return sessionPool;
		}
	protected:
		TSessionPool* sessionPool;
	};
}  // namespace FeedTheDog

