#include "Worker.h"
#include <sys/epoll.h>
#include "Loop.h"
#include "RunProxy.h"
#include "FDTaskCtlProxy.h"
#include "Task.h"
#include "Logger.h"
#include "ITcpAcceptor.h"
Worker::Worker(unsigned int core) :
	EventTaskBase(0, EventFdFlags(EventFdFlags::CloseOnExec |
		EventFdFlags::Nonblock))
{
	static int ID = 0;
	workerID_ = ID++;


	FDTaskCtlProxy::Add(loop_, EPOLLIN, this);
	worker_ = std::make_unique<std::thread>([this]() {
		RunProxy::PrepareBuffers(loop_);
		RunProxy::Start(loop_);
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
	RunProxy::Stop(loop_);
}

void Worker::DoEvent(Loop & loop, EpollOption op)
{
	assert(op.Flags.Err == false);

	Accepts tmp;
	while (!acceptFds_.empty())
	{
		if (acceptFds_.pop(tmp))
		{
			// 创建conn，回调acceptor，accept的错误cb不在这调用
			tmp.Acceptor->DoAccept(loop, tmp.Fd);
		}
	}
	DecEvent();
}

int Worker::ID() const
{
	return workerID_;
}

void Worker::Wait()
{
	worker_->join();
}
// TODO: 有没有比这更好的方法
void Worker::PostTcpFd(const int * begin, size_t size, ITcpAcceptor * acceptor)
{
	assert(acceptor != nullptr);
	Accepts act;
	act.Acceptor = acceptor;
	for (size_t i = 0; i < size; i++)
	{
		assert(*begin != -1);
		act.Fd = *begin;
		while (!acceptFds_.push(act)) {}
		++begin;
	}
	IncEvent();
}

void Worker::IncEvent()
{
	fd_.Write(1);
}

void Worker::DecEvent()
{
	eventfd_t ignore;
	fd_.Read(&ignore);
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
