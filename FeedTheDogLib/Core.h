#pragma once
namespace FeedTheDog
{
	// Service Manager & ThreadPool
	template<typename TCorePolicy>
	class Core :
		private _BOOST noncopyable
	{
	public:
		typedef typename TCorePolicy::TConfig TConfig;
		typedef typename TCorePolicy::TTraceSource TTraceSource;
		typedef typename TCorePolicy::template TCore<Core>::TWorkerType TWorker;
		typedef typename TCorePolicy::template TCore<Core>::TService TService;
		typedef typename TCorePolicy::template TCore<Core>::TWorkerVectorType TWorkerVectorType;
		Core()
		{
			isStop = true;
			tmpWorkerIndex = -1;
			config = make_unique<TConfig>();
			trace = make_unique<TTraceSource>(*config);
			GetTrace()->DebugPoint("New Core");
			threadCount = config->GetThreadCount();
			assert(threadCount > 0 && threadCount <= config->GetMaxThreadCount());

			workers=TCorePolicy::template TCore<Core>::CreateWorkerVector(this, threadCount);

			config->Save();
			GetTrace()->DebugPoint("Initialized");
		}
		virtual ~Core()
		{
			GetTrace()->DebugPoint("Free Core");
			//Stop();
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
			svr->AsyncStart();
			return true;
		}
		void Start()
		{
			GetTrace()->DebugPoint("Core Start");
			isStop = false;
			TCorePolicy::template TCore<Core>::StartWorkers(workers);
		}
		void Stop()
		{
			// 整个程序结束需要执行的东西，不管结束后再start的情况了，如果需要就重新new
			if (isStop)
			{
				return;
			}
			GetTrace()->DebugPoint("Core Stop");
			isStop = true;
			// 此处停止服务不会关掉会话
			for each (auto& var in services)
			{
				var.second->Stop();
			}
			// 此处会关并删掉会话
			TCorePolicy::template TCore<Core>::StopWorkers(workers);
		}
		int GetWorkerCount() const
		{
			return threadCount;
		}
		// 取空闲Worker
		TWorker* SelectIdleWorker()
		{
			assert(threadCount > 0);
			TWorker* result = NULL;
			if (isStop)
			{
				// 当core未开时用于分散此时添加的服务
				tmpWorkerIndex = (tmpWorkerIndex + 1) % threadCount;
				result = workers[tmpWorkerIndex].get();
			}
			else
			{
				result = TCorePolicy::template TCore<Core>::SelectIdleWorker(workers);
			}
			assert(result != NULL);
			return result;
		}
		unique_ptr<TTraceSource>& GetTrace()
		{
			return trace;
		}

	private:
		bool isStop;
		// 当core没启动时用的
		int tmpWorkerIndex;
		unique_ptr<TConfig> config;
		unique_ptr<TTraceSource> trace;
		TWorkerVectorType workers;
		concurrent_unordered_map<const char*, shared_ptr<TService>> services;
		unsigned int threadCount;
	};

}  // namespace FeedTheDog
