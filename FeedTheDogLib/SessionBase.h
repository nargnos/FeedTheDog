#pragma once
#include "Config.h"
#include "CoreTrait.h"
namespace FeedTheDog
{
	class SessionBase:
		public _BOOST noncopyable
	{
	public:
		
		typedef _STD array<unsigned char, Config::BufferSize> TBufferType;
		typedef SessionBase TSessionBase;
		typedef typename CoreTrait::TCore TCore;

		SessionBase(const shared_ptr<TCore>& corePtr, _ASIO io_service& io);
		virtual ~SessionBase();
		TBufferType& GetBuffer();
		// È¡µÃCore
		shared_ptr<TCore> GetCore() const;
		_ASIO io_service& GetIoService();
	protected:
		TBufferType buffer;
		shared_ptr<TCore> core;
		_ASIO io_service& ios;
	};
}  // namespace FeedTheDog

