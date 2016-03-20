#pragma once
#include "Config.h"
#include "CoreTrait.h"
namespace FeedTheDog
{
	class SessionBase :
		public _BOOST noncopyable
	{
	public:

		typedef _STD array<unsigned char, Config::BufferSize> TBufferType;
		typedef SessionBase TSessionBase;
		typedef typename CoreTrait::TCore TCore;
		typedef typename CoreTrait::TWorker TWorker;
		typedef TCore* TReturnCore;
		SessionBase(TWorker* worker);
		virtual ~SessionBase();
		TBufferType& GetBuffer();
		// È¡µÃCore
		TReturnCore GetCore();
		SessionBase::TWorker * SessionBase::GetWorker();
	protected:
		TWorker* worker_;
		TBufferType buffer;
		TReturnCore core;
	};
}  // namespace FeedTheDog

