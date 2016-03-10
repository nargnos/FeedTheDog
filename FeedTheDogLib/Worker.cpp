#include "stdafx.h"
#include "Worker.h"
#include "Core.h"

namespace FeedTheDog
{
	Worker::Worker(const shared_ptr<TCore>& core):
		sessionPool(core, ioService)
	{
		static int wid = 0;
		id = wid++;
		owner = core;
		//ioService = ;
		//sessionPool = make_shared<TSessionPool>(this);
	}

	Worker::~Worker()
	{
		//Stop();
	}

	Worker::TSessionPool* Worker::GetSessionPool()
	{
		return &sessionPool;
	}

	_ASIO io_service& Worker::GetIoService()
	{
		return ioService;
	}

	int Worker::GetID() const
	{
		return id;
	}

	

	void Worker::Start()
	{
		work = make_unique<_ASIO io_service::work>(ioService);

		ioService.reset();
		ioService.run();
	}
	void Worker::Stop()
	{
		work.reset();
		sessionPool.CloseAll();
		
		ioService.stop();
		
	}

	shared_ptr<Worker::TCore> Worker::GetCore() const
	{
		return owner.lock();
	}

}  // namespace FeedTheDog

