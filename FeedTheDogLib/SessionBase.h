#pragma once
namespace FeedTheDog
{
	class SessionBase :
		public _BOOST noncopyable
	{
	public:
		static const unsigned int BufferSize = 1024 * 6;
		typedef _STD array<unsigned char, BufferSize> TBufferType;
		SessionBase()
		{
		}
		virtual ~SessionBase()
		{
		}
		TBufferType& GetBuffer()
		{
			return buffer;
		}
	protected:
		TBufferType buffer;
	};

}  // namespace FeedTheDog

