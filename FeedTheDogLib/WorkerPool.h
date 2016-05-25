#pragma once
#include "Define.h"
namespace FeedTheDog
{
	enum WorkerSelector :unsigned char
	{
		// ��ѯ����һ��
		NextWorker,
		// ��ǰ�̰߳󶨵�
		CurrentWorker
	};
	class WorkerPool :
		public _BOOST noncopyable
	{
	public:
		typedef Worker TWorker;

		WorkerPool();

		~WorkerPool();
		int GetWorkerCount() const;
		using WorkerPtr = unique_ptr<TWorker, _BOOST alignment::aligned_delete>;

		template<WorkerSelector selector>
		const WorkerPtr& GetWorker() const;

		template<>
		const WorkerPtr& GetWorker<WorkerSelector::NextWorker>() const
		{
			return SelectWorker();
		}


		template<>
		const WorkerPtr& GetWorker<WorkerSelector::CurrentWorker>() const
		{
			return SelectCurrentThreadWorker();
		}

		template<typename TFunc>
		void PostTask(TFunc&& handler)
		{
			assert(!isStopped_);
			threadPool_.submit(_STD forward<TFunc>(handler));
		}

		template<typename TFunc, typename... TArgs>
		using TFuncResult =
			_STD result_of_t<_STD decay_t<TFunc>(_STD decay_t<TArgs>...)>;


		// ע���ڻص���get������
		template<typename TFunc, typename... TArgs>
		_STD future<_STD result_of_t<_STD decay_t<TFunc>(_STD decay_t<TArgs>...)>> Async(TFunc&& func, TArgs&&... args)
		{
			assert(!isStopped_);
			using TTaskType = _STD packaged_task<TFuncResult<TFunc, TArgs...>(_STD decay_t<TArgs>...)>;
			auto task = _STD make_shared<TTaskType>(_STD forward<TFunc>(func));
			auto result = task->get_future();
			PostTask(_STD bind(&TTaskType::operator(), _STD move(task), _STD forward<TArgs>(args)...));
			return result;
		}
		// ����-1��ʾ�Ҳ���
		inline int CurrentWorkerID() const;
		_ASIO io_service& GetIoService();
		friend class WorkerPoolFriendProxy;
	protected:
		// ����һ���̳߳ش�����io��һЩ�첽����
		
		static constexpr double ThreadCountScale = 1;
		void Stop();
		void Join();
		using TWorkerVectorType = _STD vector<WorkerPtr>;
		using ThreadWorkerPair = _STD pair<thread::id, int>;
		int GetWorkerIndex() const;
		// ��ѯѡ��worker
		const WorkerPtr& SelectWorker() const;
		// ѡȡ�뵱ǰ�̰߳󶨵�worker
		const WorkerPtr& SelectCurrentThreadWorker() const;
		inline int ThreadIdToWorkerId(thread::id& tid) const;

		
		int core_;
		int threadCount_;
		int threadPoolSize_;
		bool isStopped_;
		// һЩ��ͨ�Ĳ��������
		_BOOST basic_thread_pool threadPool_; // 72
		_ASIO io_service ioService_; // 12
		TWorkerVectorType workers_; // 12
		_STD vector<shared_ptr<thread>> threads_;
		_STD vector<ThreadWorkerPair> threadToWorker_;
	};


}  // namespace FeedTheDog
