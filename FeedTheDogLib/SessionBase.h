#pragma once
namespace FeedTheDog
{
	template<typename TSessionPool>
	class SessionBase :
		public _BOOST noncopyable
	{
	public:
		static const unsigned int BufferSize = 1024 * 6;
		typedef _STD array<unsigned char, BufferSize> TBufferType;
		typedef SessionBase TSessionBase;
	
		SessionBase(TSessionPool* pool) :
			sessionPool(pool)
		{
		}
		virtual ~SessionBase()
		{
		}
		TBufferType& GetBuffer()
		{
			return buffer;
		}
		
		TSessionPool * SessionBase::GetSessionPool()
		{
			return sessionPool;
		}
	protected:
		TSessionPool* sessionPool;
		TBufferType buffer;
	};
}  // namespace FeedTheDog

