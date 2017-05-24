#ifndef WORKER_H_
#define WORKER_H_
#include <memory>
#include <thread>
#include "Noncopyable.h"
#include "Loop.h"
namespace Detail
{
	class GetLoopAttorney;
	class StopWorkerAttorney;
	// 线程安全
	class Worker :
		public Noncopyable
	{
	public:
		friend GetLoopAttorney;
		friend StopWorkerAttorney;
		explicit Worker(unsigned int coreId);
		virtual ~Worker();
		int ID() const;
		void Join();
		std::thread::id Tid()const;
	private:
		Loop& GetLoop();
		Loop loop_;
		std::unique_ptr<std::thread> worker_;
		int workerID_;
	};

}  // namespace Detail

#endif // !WORKER_H_
