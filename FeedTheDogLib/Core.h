#pragma once
namespace FeedTheDog
{
	// Service Manager
	template<typename TCorePolicy>
	class Core :
		private _BOOST noncopyable
	{
	public:
		typedef typename TCorePolicy::TConfig TConfig;
		typedef typename TCorePolicy::TTraceSource TTraceSource;
		typedef typename TCorePolicy::TWorker TWorker;
		typedef typename TCorePolicy::TWorkerPool TWorkerPool;
		typedef typename TCorePolicy::template TService<Core>::TServiceType TService;

		Core() :
			isStopped(true),
			workerPool(make_unique<TWorkerPool>()),
			config(make_unique<TConfig>())
		{
			trace = make_unique<TTraceSource>(*config);
			GetTrace()->DebugPoint("New Core");

			config->Save();
			GetTrace()->DebugPoint("Initialized");
		}
		virtual ~Core()
		{
			GetTrace()->DebugPoint("Free Core");
		}

		TService* GetService(const char* name)
		{
			if (services.count(name) > 0)
			{
				return services[name].get();
			}
			return NULL;
		}
		// TODO: 测试在运行时添加、删除服务的运行情况
		bool AddService(const shared_ptr<TService>& svr)
		{
			if (services.count(svr->Name()) > 0)
			{
				return false;
			}
			if (!svr->Init(this))
			{
				return false;
			}
			services.insert({ svr->Name(), svr });
			GetTrace()->DebugPoint("Add Service", false, 0, svr->Name());
			svr->Start();
			return true;
		}
		// TODO: 运行时卸载服务
		void Start()
		{
			GetTrace()->DebugPoint("Core Start");
			isStopped = false;
			workerPool->Start();
		}
		void Stop()
		{
			// 整个程序结束需要执行的东西，不管结束后再start的情况了，如果需要就重新new
			if (isStopped)
			{
				return;
			}
			GetTrace()->DebugPoint("Core Stop");
			isStopped = true;
			// 此处停止服务不会关掉会话
			for each (auto& var in services)
			{
				var.second->Stop();
			}
			// 此处会关并删掉会话
			workerPool->Stop();
		}
		inline const unique_ptr<TTraceSource>& FASTCALL GetTrace() const
		{
			return trace;
		}
		inline const unique_ptr<TWorkerPool>& FASTCALL GetWorkerPool() const
		{
			return workerPool;
		}
	private:
		unique_ptr<TWorkerPool> workerPool; // workerPool持有ioservice，必须比service后析构，此声明位置不可调整
		concurrent_unordered_map<string, shared_ptr<TService>> services;

		unique_ptr<TConfig> config;
		unique_ptr<TTraceSource> trace;
		bool isStopped;

	};

}  // namespace FeedTheDog
