#ifndef IOSERVICE_H_
#define IOSERVICE_H_
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include "Noncopyable.h"
class Worker;
class Loop;

class AcceptorRegister;
class IoService :public Noncopyable
{
public:
	~IoService();
	friend AcceptorRegister;
	// 终止后不能重新开始
	void Shutdown();
	static std::shared_ptr<IoService> Instance();
	const std::unique_ptr<Worker>& SelectWorker() const;
	void Wait();
	size_t WorkerCount() const;
private:
	IoService();
	std::vector<std::unique_ptr<Worker>> workers_;
	std::unique_ptr<std::thread> master_;
	std::unique_ptr<Loop> loop_;
	size_t workerSize_;
	mutable size_t selectID_;
};


#endif // IOSERVICE_H_

