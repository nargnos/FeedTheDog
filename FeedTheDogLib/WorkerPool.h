#pragma once
namespace FeedTheDog
{
	class Worker;
	
	class WorkerPool :
		public _BOOST noncopyable
	{
	public:
		typedef Worker TWorker;
		typedef _STD vector<unique_ptr<TWorker>> TWorkerVectorType;
		WorkerPool();

		~WorkerPool();
		void Stop();
		void Start();
		int GetWorkerCount() const;
		// »°ø’œ–Worker
		TWorker* SelectIdleWorker() const;
		TWorker* SelectWorker() const;

	private:
		int GetWorkerIndex() const;
		TWorkerVectorType workers;
		mutable _STD atomic<int> workerIndex;
		unsigned int threadCount;
		bool isStopped;
	};
}  // namespace FeedTheDog
