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
		// TODO: 测试在运行时添加、删除服务的运行情况
		bool AddService(const shared_ptr<TService>& svr);
		// TODO: 运行时卸载服务
		void Start();
		void Stop();
		const unique_ptr<TWorkerPool>& GetWorkerPool() const;
	private:
		shared_ptr<ServiceBaseImpl> serviceBaseImpl_;
		unique_ptr<TWorkerPool> workerPool_; // workerPool持有ioservice，必须比service后析构，此声明位置不可调整
		concurrent_unordered_map<string, shared_ptr<TService>> services;

		bool isStopped;

	};

}  // namespace FeedTheDog
