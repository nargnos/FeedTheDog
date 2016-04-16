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
		// TODO: ����������ʱ��ӡ�ɾ��������������
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
			// �������������Ҫִ�еĶ��������ܽ�������start������ˣ������Ҫ������new
			if (isStop)
			{
				return;
			}
			GetTrace()->DebugPoint("Core Stop");
			isStop = true;
			// �˴�ֹͣ���񲻻�ص��Ự
			for each (auto& var in services)
			{
				var.second->Stop();
			}
			// �˴���ز�ɾ���Ự
			TCorePolicy::template TCore<Core>::StopWorkers(workers);
		}
		int GetWorkerCount() const
		{
			return threadCount;
		}
		// ȡ����Worker
		TWorker* SelectIdleWorker()
		{
			assert(threadCount > 0);
			TWorker* result = NULL;
			if (isStop)
			{
				// ��coreδ��ʱ���ڷ�ɢ��ʱ��ӵķ���
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
		// ��coreû����ʱ�õ�
		int tmpWorkerIndex;
		unique_ptr<TConfig> config;
		unique_ptr<TTraceSource> trace;
		TWorkerVectorType workers;
		concurrent_unordered_map<const char*, shared_ptr<TService>> services;
		unsigned int threadCount;
	};

}  // namespace FeedTheDog
