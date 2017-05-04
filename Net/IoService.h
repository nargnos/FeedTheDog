#ifndef IOSERVICE_H_
#define IOSERVICE_H_
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include "Noncopyable.h"
class Worker;
class Loop;

class IoService :
	public Noncopyable
{
public:
	~IoService();
	// 终止后不能重新开始
	void Shutdown();
	static std::shared_ptr<IoService> Instance();
	const std::unique_ptr<Worker>& SelectWorker();
	void Wait();
	size_t WorkerCount() const;
private:
	IoService();
	std::vector<std::unique_ptr<Worker>> workers_;
	size_t workerSize_;
	std::atomic_int_fast16_t selectID_;
};


#endif // IOSERVICE_H_

