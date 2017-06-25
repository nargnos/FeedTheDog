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
#include "GetLoopAttorney.h"
#include "TlsPackage.h"
namespace Detail
{
	Worker::Worker(unsigned int coreId)
	{
		static int ID = 0;
		workerID_ = ID++;
		worker_ = std::make_unique<std::thread>([this]() {
			RunAttorney::PrepareBuffers(loop_);
			GlobalTlsPackage::Instance().SetLoop(&loop_);
			RunAttorney::Start(loop_);
		});
		SetAffinity(worker_->native_handle(), coreId);
	}

	Worker::~Worker()
	{
		if (worker_)
		{
			worker_->join();
		}
	}

	int Worker::ID() const
	{
		assert(workerID_ != -1);
		return workerID_;
	}

	void Worker::Join()
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