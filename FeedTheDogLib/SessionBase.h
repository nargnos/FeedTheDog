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

		SessionBase(TCore* corePtr, _ASIO io_service& io);
		virtual ~SessionBase();
		TBufferType& GetBuffer();
		// ȡ��Core
		TCore* GetCore() const;
		_ASIO io_service& GetIoService();
	protected:
		TBufferType buffer;
		TCore* core;
		_ASIO io_service& ios;
		// ֻ�ڴ���ʱ��ֵfalse��ʹ�õĲ��ֲ���Ҫ�߳�ͬ��
		// ��ʾsession��û�б��رչ�������socket.is_open()
		bool isClosed;
		// ��ʾ�Ƿ��ѴӴ洢�ṹ�����ʹ��ʱ����lock������Ҳ����ͬ����
		bool isErased;
	};
}  // namespace FeedTheDog

