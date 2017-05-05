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
#include "Noncopyable.h"
#include "EventTaskBase.h"
#include "Loop.h"
class ITcpAcceptor;
class StopWorkerAttorney;
class GetLoopAttorney;
void SetAffinity(pthread_t t, unsigned int core);
// �̰߳�ȫ
class Worker :
	public Noncopyable
{
public:
	friend StopWorkerAttorney;
	friend GetLoopAttorney;
	explicit Worker(unsigned int core);
	virtual ~Worker();
	int ID()const;
	void Wait();
	std::thread::id Tid()const;
private:
	Loop& GetLoop();
	// �ᵼ��δ������񱻶���
	void Stop();
	Loop loop_;
	std::unique_ptr<std::thread> worker_;
	int workerID_;
};


#endif // !WORKER_H_
