#pragma once
#ifndef WORKER_H_
#define WORKER_H_
#include <memory>
#include <thread>
#include <atomic>
#include <queue>
#include <tuple>
#include <vector>
#include <cassert>
#include <boost/lockfree/queue.hpp>
#include "Noncopyable.h"
#include "EventTaskBase.h"
#include "Loop.h"
class ITcpAcceptor;
class StopWorkerProxy;
void SetAffinity(pthread_t t, unsigned int core);
// 线程安全
class Worker :
	public EventTaskBase
{
public:
	friend StopWorkerProxy;
	explicit Worker(unsigned int core);
	virtual ~Worker();
	int ID()const;
	void Wait();
	// TODO: 这个不应该在这里, 要把这个封装成类，再注册到worker作为线程安全的投递，注册acc的同时注册投递任务
	// 不过这样会对每个acc产生n个eventfd，它应该和worker数相同，先封装，这里提供线程安全的任务投递
	void PostTcpFd(const int* begin, size_t size, ITcpAcceptor* acceptor);
private:
	// 会导致未完成任务被丢弃
	void Stop();
	virtual void DoEvent(Loop& loop, EpollOption op);
	void IncEvent();
	void DecEvent();
	struct Accepts
	{
		int Fd;
		ITcpAcceptor* Acceptor;
	};
	// FIX: 这个放到loop里就不需要eventfd，但是这样逻辑不对，需要修改
	// 不过post的时候无法唤醒wait(-1),还是要eventfd；pipe不考虑
	boost::lockfree::queue<Accepts> acceptFds_;
	Loop loop_;
	std::unique_ptr<std::thread> worker_;
	int workerID_;
};


#endif // !WORKER_H_
