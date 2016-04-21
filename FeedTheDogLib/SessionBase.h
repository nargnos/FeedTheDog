#pragma once
namespace FeedTheDog
{
	class SessionBase :
		public _BOOST noncopyable
	{
	public:
		enum
		{
			BufferSize = 1024 * 8
		};
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

