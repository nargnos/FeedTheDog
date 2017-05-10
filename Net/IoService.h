#ifndef IOSERVICE_H_
#define IOSERVICE_H_
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include "Noncopyable.h"
namespace Detail
{
	class Worker;
	class Loop;
	class GetWorkersAttorney;
	struct PerfInfo
	{
		// ���е��̵߳�loop�ṹ
		Loop* __restrict IdleLoop;
		// ��æ���̵߳�loop�ṹ
		Loop* __restrict BusyLoop;
		size_t IdleCount;
		size_t BusyCount;
		// ��������� �������
		size_t TaskCount;
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
		// ������֤�����ȷ
		const PerfInfo& PerformanceSnapshot();
	private:
		IoService();
		const std::vector<std::unique_ptr<Worker>>& Workers() const;
		std::vector<std::unique_ptr<Worker>> workers_;
		size_t workerSize_;
	};


}  // namespace Detail
using Detail::IoService;
#endif // IOSERVICE_H_

