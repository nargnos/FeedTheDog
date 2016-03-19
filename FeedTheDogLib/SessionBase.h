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
		SessionBase(TWorker* worker);
		virtual ~SessionBase();
		TBufferType& GetBuffer();
		// ȡ��Core
		shared_ptr<TCore>& GetCore();
		_ASIO io_service& GetIoService();
		SessionBase::TWorker * SessionBase::GetWorker();
		_BOOST system::error_code& GetErrorCode();
	protected:
		TWorker* worker_;
		TBufferType buffer;
		shared_ptr<TCore> core;
		_ASIO io_service& ios;
		// ��ʾ�Ƿ��ѴӴ洢�ṹ�����ʹ��ʱ����lock������Ҳ����ͬ����
		bool isErased;
		//_ASIO deadline_timer timer;
		_BOOST system::error_code errorCode;
	};
}  // namespace FeedTheDog

