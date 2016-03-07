#pragma once
#include "..\Common.h"
#include "..\Trait\SessionPoolTrait.h"
#include "..\Trait\CoreTrait.h"

namespace FeedTheDog
{
	class SessionPool
	{
	public:
		typedef SessionPool TSessionPool;
		typedef typename SessionPoolTrait::TTcpSession TTcpSession;
		typedef typename SessionPoolTrait::TUdpSession TUdpSession;
		typedef typename CoreTrait::TCore TCore;


		explicit SessionPool(TCore* corePtr) :
			count(0),
			core(corePtr),
			tcpSessionPool(tcpAlloc, 0),
			udpSessionPool(udpAlloc, 0)
		{
		}
		virtual ~SessionPool()
		{
			assert(count == 0);
		}

		int Count() const
		{
			return count;
		}
		shared_ptr<TTcpSession> GetTcpSession()
		{
			assert(core);
			auto result = tcpSessionPool.construct<true, false>(core);
			++count;
			return shared_ptr<TTcpSession>(result, _BOOST bind(&TSessionPool::TcpFree, this, _1));
		}
		shared_ptr<TUdpSession> GetUdpSession()
		{
			assert(core);
			auto result = udpSessionPool.construct<true, false>(core);
			++count;
			return shared_ptr<TUdpSession>(result, _BOOST bind(&TSessionPool::UdpFree, this, _1));
		}

	private:
		void TcpFree(TTcpSession* ptr)
		{
			tcpSessionPool.destruct<true>(ptr);
			--count;
		}
		void UdpFree(TUdpSession* ptr)
		{
			udpSessionPool.destruct<true>(ptr);
			--count;
		}
		_BOOST atomic<int> count;
		TCore* core;
		_STD allocator<TTcpSession> tcpAlloc;
		_STD allocator<TUdpSession> udpAlloc;

		_BOOST lockfree::detail::freelist_stack<TTcpSession> tcpSessionPool;
		_BOOST lockfree::detail::freelist_stack<TUdpSession> udpSessionPool;
	};
}  // namespace FeedTheDog