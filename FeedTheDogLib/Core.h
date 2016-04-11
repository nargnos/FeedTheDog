#pragma once
namespace FeedTheDog
{
	// Service Manager & ThreadPool
	template<typename Trait>
	class Core :
		private _BOOST noncopyable
	{
	public:
		typedef typename Core TCore;
		typedef typename Trait::TConfig TConfig;
		typedef typename Trait::TTraceSource TTraceSource;
		typedef typename Trait::template TCore<Core>::TWorkerType TWorker;
		typedef typename Trait::template TCore<Core>::TService TService;
		Core()
		{
			isStop = true;
			tmpWorkerIndex = -1;
			config = make_unique<TConfig>();

			config->Load();
			GetTrace()->DebugPoint(LogMsg::NewCore);
			threadCount = config->GetThreadCount();
			assert(threadCount > 0 && threadCount <= config->GetMaxThreadCount());

			for (size_t i = 0; i < threadCount; i++)
			{
				workers.push_back(make_unique<TWorker>(this));
			}
			config->Save();
			GetTrace()->DebugPoint(LogMsg::Initialized);
		}
		virtual ~Core()
		{
			GetTrace()->DebugPoint(LogMsg::FreeCore);
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
			services.insert(_STD pair<const char*, shared_ptr<TService>>(svr->Name(), svr));
			GetTrace()->DebugPoint(LogMsg::AddService, false, 0, svr->Name());
			svr->AsyncStart();
			return true;
		}
		//void DeleteService(const shared_ptr<TService>&);
		//void Core::DeleteService(const shared_ptr<TService>& svr)
		//{
		//	auto find = GetService(svr->Name());
		//	if (find == NULL)
		//	{
		//		return;
		//	}
		//	// �ɷ�������Ƿ����������
		//	find->Stop();
		//	GetTrace()->DebugPoint(LogMsg::AddService, false, 0, svr->Name());
		//	// �Ѷ�Ӧ���������session�ص�
		//	for each (auto& var in workers)
		//	{
		//		//var->RemoveAllServiceSession(svr->Name());
		//	}
		//	mutex.lock();
		//	services.unsafe_erase(svr->Name());
		//	mutex.unlock();
		//}
		void Start()
		{
			GetTrace()->DebugPoint(LogMsg::CoreStart);
			/*for each (auto& var in services)
			{
			var.second->AsyncStart();
			}*/
			isStop = false;
			_STD vector<shared_ptr<_BOOST thread>> threads;
			assert(threadCount == workers.size());
			if (threadCount > 1)
			{
				for (size_t i = 1; i < threadCount; i++)
				{
					auto tmpThread = make_shared<_BOOST thread>(_BOOST bind(&TWorker::Start, workers[i].get()));

					threads.push_back(tmpThread);
				}
			}

			// �����߳�Ҳ������
			workers[0]->Start();
			// join�ȴ������߳̽���
			for each (auto& var in threads)
			{
				var->join();
			}
		}
		void Stop()
		{
			// �������������Ҫִ�еĶ��������ܽ�������start������ˣ������Ҫ������new
			if (isStop)
			{
				return;
			}
			GetTrace()->DebugPoint(LogMsg::CoreStop);
			isStop = true;
			// �˴�ֹͣ���񲻻�ص��Ự
			for each (auto& var in services)
			{
				var.second->Stop();
			}
			// �˴���ز�ɾ���Ự
			for (size_t i = 0; i < threadCount; i++)
			{
				workers[i]->Stop();
			}
		}
		int GetWorkerCount() const
		{
			return workers.size();
		}
		// ȡ����Worker
		TWorker* SelectIdleWorker()
		{
			assert(workers.size() > 0);
			TWorker* result = NULL;
			if (isStop)
			{
				// ��coreδ��ʱ���ڷ�ɢ��ʱ��ӵķ���
				tmpWorkerIndex = (tmpWorkerIndex + 1) % workers.size();
				result = workers[tmpWorkerIndex].get();
			}
			else
			{
				auto i = workers.begin();
				result = i->get();
				unsigned int sessionCount = result->GetSessionCount();
				++i;
				auto& end = workers.end();
				for (; i < end; ++i)
				{
					auto tmpSessionCount = (*i)->GetSessionCount();
					if (tmpSessionCount < sessionCount)
					{
						result = i->get();
						sessionCount = tmpSessionCount;
					}
				}
			}
			assert(result != NULL);
			return result;
		}
		shared_ptr<TTraceSource>& GetTrace()
		{
			return config->GetTrace();
		}

	private:
		bool isStop;
		// ��coreû����ʱ�õ�
		int tmpWorkerIndex;
		unique_ptr<TConfig> config;
		_STD vector<unique_ptr<TWorker>> workers;
		// _BOOST mutex mutex;
		concurrent_unordered_map<const char*, shared_ptr<TService>> services;
		unsigned int threadCount;
	};

}  // namespace FeedTheDog
