#ifndef IOSERVICE_H_
#define IOSERVICE_H_
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include "Noncopyable.h"
class Worker;
class Loop;
class GetWorkersAttorney;
class IoService :
	public Noncopyable
{
public:
	friend GetWorkersAttorney;
	~IoService();
	// 终止后不能重新开始
	void Shutdown();
	static std::shared_ptr<IoService> Instance();
	void Wait();
	size_t WorkerCount() const;
private:
	IoService();
	const std::vector<std::unique_ptr<Worker>>& Workers() const;
	std::vector<std::unique_ptr<Worker>> workers_;
	size_t workerSize_;
};


#endif // IOSERVICE_H_

