#pragma once
#include "..\..\Common.h"
#include "..\..\Config.h"
#include "..\..\Trait\CoreTrait.h"
namespace FeedTheDog
{
	class SessionBase:
		public _BOOST noncopyable
	{
	public:
		
		typedef _STD array<unsigned char, Config::BufferSize> TBufferType;
		typedef SessionBase TSessionBase;
		typedef typename CoreTrait::TSessionPool TSessionPool;
		typedef typename CoreTrait::TCore TCore;
		friend TSessionPool;

		SessionBase(TSessionPool* ptr);
		virtual ~SessionBase();
		TBufferType& GetBuffer();
		// 取得分配这个对象的对象池
		shared_ptr<TSessionPool>& GetIdleSessionPool();
		// 取得Core
		TCore* GetCore() const;
	protected:
		TBufferType buffer;
		shared_ptr<TSessionPool> pool;
	};
}  // namespace FeedTheDog

