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
struct PerfInfo
{
	PerfInfo(size_t count, Loop& idle, Loop& busy, size_t idleCount, size_t busyCount);

	// ���е��̵߳�loop�ṹ
	Loop& IdleLoop;
	// ��æ���̵߳�loop�ṹ
	Loop& BusyLoop;
	// ��������� �������
	size_t TaskCount;
	size_t IdleCount;
	size_t BusyCount;
};
class IoService :
	public Noncopyable
{
public:
	friend GetWorkersAttorney;
	~IoService();
	// ��ֹ�������¿�ʼ
	void Shutdown();
	static std::shared_ptr<IoService> Instance();
	void Wait();
	size_t WorkerCount() const;
	// FIX: ������һЩ�ò�������Ϣ����֮�������û�ÿ��Կ���ȥ��
	PerfInfo PerformanceSnapshot();
private:
	IoService();
	const std::vector<std::unique_ptr<Worker>>& Workers() const;
	std::vector<std::unique_ptr<Worker>> workers_;
	size_t workerSize_;
};


#endif // IOSERVICE_H_

