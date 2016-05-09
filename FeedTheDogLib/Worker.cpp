#include "stdafx.h"
#include "Worker.h"
#include "SessionPool.h"
namespace FeedTheDog
{
	Worker::Worker()
	{

		ForEachTypes<
			TcpSession,
			UdpSession,
			TcpSessionNoBuffer,
			UdpSessionNoBuffer
		>::Run(CreatePools(*this));

		ForEachTypes<Tcp, Udp>::Run(CreateResolvers(*this));
	}
	Worker::~Worker()
	{
	}
}  // namespace FeedTheDog

