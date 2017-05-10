#include "Worker.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "Loop.h"
#include "RunAttorney.h"
#include "FDTaskCtlAttorney.h"
#include "Task.h"
#include "Logger.h"
#include "Loop.h"
#include "Util.h"
namespace Detail
{
	Worker::Worker(unsigned int coreId)
	{
		static int ID = 0;
		workerID_ = ID++;
		worker_ = std::make_unique<std::thread>([this]() {
			RunAttorney::PrepareBuffers(GetLoop());
			RunAttorney::Start(GetLoop());
		});
		SetAffinity(worker_->native_handle(), coreId);
	}

	Worker::~Worker()
	{
		assert(GetLoop().State() == Loop::LoopState::Stopped);

		if (worker_)
		{
			worker_->join();
		}
	}


	void Worker::Stop()
	{
		RunAttorney::Stop(GetLoop());
	}

	int Worker::ID() const
	{
		assert(workerID_ != -1);
		return workerID_;
	}

	void Worker::Wait()
	{
		assert(worker_);
		worker_->join();
	}

	std::thread::id Worker::Tid() const
	{
		assert(worker_);
		return worker_->get_id();
	}

	Loop & Worker::GetLoop()
	{
		return loop_;
	}



}  // namespace Detail