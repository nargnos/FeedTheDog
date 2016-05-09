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
			svr->Start();
			return true;
		}
		// TODO: ����ʱж�ط���
		void Start()
		{
			GetTrace()->DebugPoint("Core Start");
			isStopped = false;
			workerPool->Start();
		}
		void Stop()
		{
			// �������������Ҫִ�еĶ��������ܽ�������start������ˣ������Ҫ������new
			if (isStopped)
			{
				return;
			}
			GetTrace()->DebugPoint("Core Stop");
			isStopped = true;
			// �˴�ֹͣ���񲻻�ص��Ự
			for each (auto& var in services)
			{
				var.second->Stop();
			}
			// �˴���ز�ɾ���Ự
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
		unique_ptr<TWorkerPool> workerPool; // workerPool����ioservice�������service��������������λ�ò��ɵ���
		concurrent_unordered_map<string, shared_ptr<TService>> services;

		unique_ptr<TConfig> config;
		unique_ptr<TTraceSource> trace;
		bool isStopped;

	};

}  // namespace FeedTheDog
