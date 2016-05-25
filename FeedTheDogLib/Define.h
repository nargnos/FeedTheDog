#pragma once
namespace FeedTheDog
{
	// buffer��С�ڴ�����
	enum
	{
		BufferSize = 1024 * 10,

		TcpPoolNextSize = 512,
		UdpPoolNextSize = 256,
		BufferPoolNextSize = TcpPoolNextSize,
	};

	__interface IService;
	class Core;
	class Worker;

	class Config;
	class WorkerPool;
	class ServiceBaseImpl;
	template<typename>
	class TraceSourceImpl;
}  // namespace FeedTheDog
