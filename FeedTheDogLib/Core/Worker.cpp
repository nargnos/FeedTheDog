#include "Worker.h"
#include "Core.h"
#include "..\Pool\SessionPool.h"
namespace FeedTheDog
{
	Worker::Worker(TCore* core)
	{
		static int wid = 0;
		id = wid++;
		owner = core;
		ioService = make_unique<_ASIO io_service>();
		sessionPool = make_shared<TSessionPool>(this);
	}

	Worker::~Worker()
	{
		//Stop();
	}

	Worker::TSessionPool* Worker::GetIdleSessionPool()
	{
		return sessionPool.get();
	}

	_ASIO io_service* Worker::GetIoService()
	{
		return ioService.get();
	}

	int Worker::GetID() const
	{
		return id;
	}

	

	void Worker::Start()
	{
		work = make_unique<_ASIO io_service::work>(*ioService);
		//ioService.reset();
		ioService->run();
	}
	void Worker::Stop()
	{
		work.reset();
		ioService->stop();
	}

	Worker::TCore * Worker::GetCore() const
	{
		return owner;
	}

}  // namespace FeedTheDog

