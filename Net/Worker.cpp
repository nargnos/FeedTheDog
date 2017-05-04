#include "Worker.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include "Loop.h"
#include "RunAttorney.h"
#include "FDTaskCtlAttorney.h"
#include "Task.h"
#include "Logger.h"
#include "ITcpAcceptor.h"
Worker::Worker(unsigned int core)
{
	static int ID = 0;
	workerID_ = ID++;

	worker_ = std::make_unique<std::thread>([this]() {
		RunAttorney::PrepareBuffers(loop_);
		RunAttorney::Start(loop_);
	});
	SetAffinity(worker_->native_handle(), core);
}


Worker::~Worker()
{
	assert(loop_.State() == Loop::LoopState::Stopped);
	worker_->join();
}


void Worker::Stop()
{
	RunAttorney::Stop(loop_);
}

int Worker::ID() const
{
	return workerID_;
}

void Worker::Wait()
{
	worker_->join();
}

std::thread::id Worker::Tid() const
{
	return worker_->get_id();
}

Loop & Worker::GetLoop()
{
	return loop_;
}

void SetAffinity(pthread_t t, unsigned int core)
{
	cpu_set_t set;

	CPU_ZERO(&set);
	CPU_SET(core, &set);

	if (pthread_setaffinity_np(t, sizeof(set), &set) == -1)
	{
		TRACEERRNO(LogPriority::Warning);
	}
}
