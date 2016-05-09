#pragma once
#include "Define.h"
#include "ForEachTypes.h"
#include "WorkerBase.h"
#include "Session.h"
#include "SessionPool.h"
namespace FeedTheDog
{
	class Worker :
		public WorkerBase
	{
	public:
		using Tcp = _ASIO ip::tcp;
		using Udp = _ASIO ip::udp;

		template<typename TProtocol>
		using TResolver = typename TProtocol::resolver;

		template<typename TProtocol, bool hasBuffer>
		using TSession = Session<TProtocol, hasBuffer>;

		template<bool hasBuffer>
		using TTcpSession = TSession<Tcp, hasBuffer>;
		using TcpSession = TTcpSession<true>;
		using TcpSessionNoBuffer = TTcpSession<false>;

		template<bool hasBuffer>
		using TUdpSession = TSession<Udp, hasBuffer>;
		using UdpSession = TUdpSession<true>;
		using UdpSessionNoBuffer = TUdpSession<false>;

		// 修改SessionPool类型在此处
		template<typename TSession>
		using TSessionPool = SessionPool<TSession>;


		using TcpSessionPool = TSessionPool<TcpSession>;
		using UdpSessionPool = TSessionPool<UdpSession>;

		using TcpSessionPoolNoBuffer = TSessionPool<TcpSessionNoBuffer>;
		using UdpSessionPoolNoBuffer = TSessionPool<UdpSessionNoBuffer>;



		Worker();
		~Worker();

		
		template<typename TProtocol>
		const unique_ptr<TResolver<TProtocol>>& FASTCALL GetResolver();

		template<>
		const unique_ptr<TResolver<Tcp>>& FASTCALL GetResolver<Tcp>()
		{
			return tcpResolver;
		}
		template<>
		const unique_ptr<TResolver<Udp>>& FASTCALL GetResolver<Udp>()
		{
			return udpResolver;
		}
		friend class WorkerFriendProxy;
	protected:
		unique_ptr<TcpSessionPool> tcpSessionPool;
		unique_ptr<UdpSessionPool> udpSessionPool;

		unique_ptr<TcpSessionPoolNoBuffer> tcpSessionPoolNoBuffer;
		unique_ptr<UdpSessionPoolNoBuffer> udpSessionPoolNoBuffer;

		unique_ptr<TResolver<Tcp>> tcpResolver;
		unique_ptr<TResolver<Udp>> udpResolver;



		class CreateResolvers
		{
		public:
			CreateResolvers(Worker& worker) :
				worker_(worker)
			{}
			template<typename TProtocol>
			void Do()
			{
				using TResolverType = TResolver<TProtocol>;
				auto& ptr = const_cast<unique_ptr<TResolverType>&>(worker_.GetResolver<TProtocol>());
				ptr = _STD move(make_unique<TResolverType>(_STD ref(worker_.ioService_)));
			}
		protected:
			Worker& worker_;
		};

		
		template<typename TSessionType>
		inline const unique_ptr<TSessionPool<TSessionType>>& GetSessionPool() const;

		template<>
		inline const unique_ptr<TcpSessionPool>& GetSessionPool<TcpSession>() const
		{
			return tcpSessionPool;
		}
		template<>
		inline const unique_ptr<UdpSessionPool>& GetSessionPool<UdpSession>() const
		{
			return udpSessionPool;
		}

		template<>
		inline const unique_ptr<TcpSessionPoolNoBuffer>& GetSessionPool<TcpSessionNoBuffer>() const
		{
			return tcpSessionPoolNoBuffer;
		}
		template<>
		inline const unique_ptr<UdpSessionPoolNoBuffer>& GetSessionPool<UdpSessionNoBuffer>() const
		{
			return udpSessionPoolNoBuffer;
		}

		// Udp用得比较少，不分太多内存
		template<typename TSessionType>
		struct SessionTrait;

		template<>
		struct SessionTrait<TcpSession>
		{
			static constexpr size_t PoolSize = 2048;
		};
		template<>
		struct SessionTrait<UdpSession>
		{
			static constexpr size_t PoolSize = 128;
		};
		template<>
		struct SessionTrait<TcpSessionNoBuffer>
		{
			static constexpr size_t PoolSize = 1024;
		};
		template<>
		struct SessionTrait<UdpSessionNoBuffer>
		{
			static constexpr size_t PoolSize = 256;
		};

		class CreatePools
		{
		public:
			CreatePools(Worker& worker):
				worker_(worker)
			{}
			template<typename TSessionType>
			void Do()
			{
				using TPoolType = TSessionPool<TSessionType>;
				auto& ptr = const_cast<unique_ptr<TPoolType>&>(worker_.GetSessionPool<TSessionType>());
				// 两个参数都已在父类初始化
				ptr = _STD move(make_unique<TPoolType>(worker_.ioService_, SessionTrait<TSessionType>::PoolSize, worker_.counter_));
			}
		protected:
			Worker& worker_;
		};
	};


}  // namespace FeedTheDog
