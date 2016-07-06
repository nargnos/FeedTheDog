#pragma once
#include "Define.h"
#include "WorkerBase.h"
#include "ObjectPool.h"
#include "Session.h"
namespace FeedTheDog
{
	enum BufferType
	{
		Array,
		Vector
	};

	struct WorkerDefines
	{
		// 池锁，设置为空，省掉开销，互斥部分用异步取worker处理。。。 这个不行，acceptor结束时持有的session不会在分配它的worker析构
		// using PoolLock = _BOOST details::pool::null_mutex;
		using PoolLock = SpinLock;
		template<typename TProtocol>
		using TResolver = typename TProtocol::resolver;

		// 需要object_pool管理socket析构
		template<typename TProtocol>
		using TSocketPool = ObjectPool<typename TProtocol::socket, _BOOST object_pool, PoolLock>;

		using TcpSocketPool = TSocketPool<Tcp>;
		using UdpSocketPool = TSocketPool<Udp>;

		using ArrayBuffer = _STD array<unsigned char, BufferSize>;
		using VectorBuffer = _STD vector<unsigned char, _BOOST container::allocator<unsigned char>>;

		// 析不析构无所谓，池析构时不会有泄露
		using ArrayPool = ObjectPool<ArrayBuffer, _BOOST pool, PoolLock>;
		using VectorPool = ObjectPool<VectorBuffer, _BOOST pool, PoolLock>;

		// BufferType::Array -> ArrayPool
		// BufferType::Vector -> VectorPool
		// 错误值 -> void
		template<BufferType bufferType>
		using TBufferPoolType =
			typename _STD _If<
			bufferType == BufferType::Array,
			ArrayPool,
			typename _STD _If<
			bufferType == BufferType::Vector,
			VectorPool,
			void
			>::type
			>::type;

		template<typename TProtocol>
		using TSession = Session<TProtocol>;

		using TcpSession = TSession<Tcp>;
		using UdpSession = TSession<Udp>;

		// 需要object_pool管理析构
		template<typename TProtocol>
		using TSessionPool = ObjectPool<TSession<TProtocol>, _BOOST object_pool, PoolLock>;

		using TcpSessionPool = TSessionPool<Tcp>;
		using UdpSessionPool = TSessionPool<Udp>;

		using TcpSessionPointer = TcpSessionPool::ObjectPointer;
		using UdpSessionPointer = UdpSessionPool::ObjectPointer;
	};



	class Worker :
		public WorkerBase,
		public WorkerDefines
	{
	public:

		Worker();
		~Worker();

		template<typename TProtocol>
		const unique_ptr<TResolver<TProtocol>>& GetResolver();

		template<>
		const unique_ptr<TResolver<Tcp>>& GetResolver<Tcp>()
		{
			return tcpResolver_;
		}
		template<>
		const unique_ptr<TResolver<Udp>>& GetResolver<Udp>()
		{
			return udpResolver_;
		}
		// 非线程安全
		template<typename TProtocol, SmartPtrTypeID ptrType = SmartPtrTypeID::Unique>
		typename TSocketPool<TProtocol>::template TObjectPtr<ptrType> NewSocket()
		{
			return GetSocketPool<TSocketPool<TProtocol>>()->Construct<ptrType>(ioService_);
		}
		// 非线程安全
		template<BufferType bufferType = BufferType::Array, SmartPtrTypeID ptrType = SmartPtrTypeID::Unique>
		typename TBufferPoolType<bufferType>::template TObjectPtr<ptrType> NewBuffer()
		{
			return GetBufferPool<bufferType>()->Construct<ptrType>();
		}
		// 非线程安全
		template<SmartPtrTypeID ptrType = SmartPtrTypeID::Unique>
		typename TBufferPoolType<Vector>::template TObjectPtr<ptrType> NewBuffer(size_t size)
		{
			return GetBufferPool<Vector>()->Construct<ptrType>(size);
		}
		// 非线程安全
		template<typename TProtocol, SmartPtrTypeID ptrType = SmartPtrTypeID::Unique>
		typename TSessionPool<TProtocol>::template TObjectPtr<ptrType> NewSession()
		{
			return GetSessionPool<TSessionPool<TProtocol>>()->Construct<ptrType>(ioService_);
		}
	protected:
		template<typename TSocketPool>
		inline const unique_ptr<TSocketPool>& GetSocketPool() const;

		template<>
		inline const unique_ptr<TcpSocketPool>& GetSocketPool<TcpSocketPool>() const
		{
			return tcpSocketPool_;
		}
		template<>
		inline const unique_ptr<UdpSocketPool>& GetSocketPool<UdpSocketPool>() const
		{
			return udpSocketPool_;
		}

		template<BufferType bufferType>
		inline const unique_ptr<TBufferPoolType<bufferType>>& GetBufferPool() const;

		template<>
		inline const unique_ptr<ArrayPool>& GetBufferPool<BufferType::Array>() const
		{
			return arrayPool_;
		}

		template<>
		inline const unique_ptr<VectorPool>& GetBufferPool<BufferType::Vector>() const
		{
			return vectorPool_;
		}


		template<typename TPool>
		inline const unique_ptr<TPool>& GetSessionPool() const;

		template<>
		inline const unique_ptr<TcpSessionPool>& GetSessionPool<TcpSessionPool>() const
		{
			return tcpSessionPool_;
		}
		template<>
		inline const unique_ptr<UdpSessionPool>& GetSessionPool<UdpSessionPool>() const
		{
			return udpSessionPool_;
		}

		unique_ptr<ArrayPool> arrayPool_;
		unique_ptr<VectorPool> vectorPool_;

		unique_ptr<TcpSessionPool> tcpSessionPool_;
		unique_ptr<UdpSessionPool> udpSessionPool_;

		unique_ptr<TcpSocketPool> tcpSocketPool_;
		unique_ptr<UdpSocketPool> udpSocketPool_;

		unique_ptr<TResolver<Tcp>> tcpResolver_;
		unique_ptr<TResolver<Udp>> udpResolver_;

	};


}  // namespace FeedTheDog
