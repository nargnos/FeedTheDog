#include "stdafx.h"
#include "Worker.h"
#include "SessionPool.h"
namespace FeedTheDog
{
	Worker::Worker()
	{
		tcpSessionPool = make_unique<TTcpSessionPool>(ioService);
		udpSessionPool = make_unique<TUdpSessionPool>(ioService);
	}
	Worker::~Worker()
	{
	}
	unsigned int Worker::GetSessionCount() const
	{
		return tcpSessionPool->GetSessionCount() +
			udpSessionPool->GetSessionCount();
	}
	void Worker::CloseAllSessions()
	{
		tcpSessionPool->__PreDestruct();
		tcpSessionPool->CloseAll();

		udpSessionPool->__PreDestruct();
		udpSessionPool->CloseAll();

	}

}  // namespace FeedTheDog

