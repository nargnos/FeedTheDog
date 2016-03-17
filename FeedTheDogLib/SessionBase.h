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
		SessionBase(TWorker* worker, _ASIO io_service& io);
		virtual ~SessionBase();
		TBufferType& GetBuffer();
		// ȡ��Core
		shared_ptr<TCore>& GetCore();
		_ASIO io_service& GetIoService();
		void Close();
		_ASIO deadline_timer& GetTimer();
		SessionBase::TWorker * SessionBase::GetWorker();
	protected:
		TWorker* worker_;
		TBufferType buffer;
		shared_ptr<TCore> core;
		_ASIO io_service& ios;
		// ��ʾ�Ƿ��ѴӴ洢�ṹ�����ʹ��ʱ����lock������Ҳ����ͬ����
		bool isErased;
		_ASIO deadline_timer timer;
		boost::system::error_code ignored_ec;
	};
}  // namespace FeedTheDog

