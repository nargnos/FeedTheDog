#include "stdafx.h"
#include "ForEachTypes.h"
#include "Worker.h"
namespace FeedTheDog
{
	
	Worker::Worker() :
		tcpResolver_(make_unique<TResolver<Tcp>>(ioService_)),
		udpResolver_(make_unique<TResolver<Udp>>(ioService_)),
		tcpSocketPool_(make_unique<TcpSocketPool>(TcpPoolNextSize)),
		udpSocketPool_(make_unique<UdpSocketPool>(UdpPoolNextSize)),
		arrayPool_(make_unique<ArrayPool>(BufferPoolNextSize)),
		vectorPool_(make_unique<VectorPool>(BufferPoolNextSize)),
		tcpSessionPool_(make_unique<TcpSessionPool>(TcpPoolNextSize)),
		udpSessionPool_(make_unique<UdpSessionPool>(UdpPoolNextSize))
	{
		
	}
	Worker::~Worker()
	{
	}
}  // namespace FeedTheDog

