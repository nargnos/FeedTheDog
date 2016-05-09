#pragma once
namespace FeedTheDog
{
	// TODO: ��һ����ʲô��ʽ���Ըĳ���concurrency�ķ�ʽ
	class Worker;

	class WorkerPool :
		public _BOOST noncopyable
	{
	public:
		typedef Worker TWorker;

		WorkerPool();

		~WorkerPool();
		void Stop();
		void Start();
		int GetWorkerCount() const;
		// ȡ����Worker
		TWorker* FASTCALL SelectIdleWorker() const;
		TWorker* FASTCALL SelectWorker() const;

	private:
		using WorkerPtr = unique_ptr<TWorker, _BOOST alignment::aligned_delete>;
		using TWorkerVectorType = _STD vector<WorkerPtr>;
		int GetWorkerIndex() const;

		ALIGN mutable _STD atomic<int> workerIndex_;
		TWorkerVectorType workers_;
		TWorkerVectorType::iterator workersBegin_;
		TWorkerVectorType::iterator workersEnd_;
		unsigned int core_;
		unsigned int threadCount_;
		bool isStopped_;
	};
}  // namespace FeedTheDog
