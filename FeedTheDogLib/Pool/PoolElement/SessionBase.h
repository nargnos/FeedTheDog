#pragma once
#include "..\..\Common.h"
#include "..\..\Trait\CoreTrait.h"

namespace FeedTheDog
{
	class SessionBase:
		public _BOOST noncopyable
	{
	public:
		static const int BufferSize = 1024 * 10;
		typedef _STD array<unsigned char, BufferSize> TBufferType;

		typedef SessionBase TSessionBase;
		typedef typename CoreTrait::TCore TCore;
		SessionBase(TCore* corePtr);
		virtual ~SessionBase() = default;

		TBufferType& GetBuffer();

	protected:
		TBufferType buffer;
		TCore* core;
	};
}  // namespace FeedTheDog

