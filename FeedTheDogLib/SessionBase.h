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
		// 取得Core
		TCore* GetCore() const;
		_ASIO io_service& GetIoService();
	protected:
		TBufferType buffer;
		TCore* core;
		_ASIO io_service& ios;
		// 只在创建时赋值false，使用的部分不需要线程同步
		// 表示session有没有被关闭过，不用socket.is_open()
		bool isClosed;
		// 表示是否已从存储结构清除，使用时都在lock，这里也不需同步了
		bool isErased;
	};
}  // namespace FeedTheDog

