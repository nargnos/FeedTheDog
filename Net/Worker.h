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
// �̰߳�ȫ
class Worker :
	public EventTaskBase
{
public:
	friend StopWorkerProxy;
	explicit Worker(unsigned int core);
	virtual ~Worker();
	int ID()const;
	void Wait();
	void PostTcpFd(const int* begin, size_t size, ITcpAcceptor* acceptor);
private:
	// �ᵼ��δ������񱻶���
	void Stop();
	virtual void DoEvent(Loop& loop, EpollOption op);
	void IncEvent();
	void DecEvent();
	struct Accepts
	{
		int Fd;
		ITcpAcceptor* Acceptor;
	};
	// FIX: ����ŵ�loop��Ͳ���Ҫeventfd�����������߼����ԣ���Ҫ�޸�
	// ����post��ʱ���޷�����wait(-1),����Ҫeventfd��pipe������
	boost::lockfree::queue<Accepts> acceptFds_;
	Loop loop_;
	std::unique_ptr<std::thread> worker_;
	int workerID_;
};


#endif // !WORKER_H_
