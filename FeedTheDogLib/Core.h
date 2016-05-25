#pragma once
#include "Define.h"
namespace FeedTheDog
{
	// Service Manager
	class Core :
		public _BOOST noncopyable,
		public _STD enable_shared_from_this<Core>
	{
	public:

		typedef Worker TWorker;
		typedef IService TService;
		typedef WorkerPool TWorkerPool;
		Core();
		virtual ~Core();

		const shared_ptr<Core::TService>& GetService(const char* name);
		// TODO: ����������ʱ��ӡ�ɾ��������������
		bool AddService(const shared_ptr<TService>& svr);
		// TODO: ����ʱж�ط���
		void Start();
		void Stop();
		const unique_ptr<TWorkerPool>& GetWorkerPool() const;
	private:
		shared_ptr<ServiceBaseImpl> serviceBaseImpl_;
		unique_ptr<TWorkerPool> workerPool_; // workerPool����ioservice�������service��������������λ�ò��ɵ���
		concurrent_unordered_map<string, shared_ptr<TService>> services;

		bool isStopped;

	};

}  // namespace FeedTheDog
