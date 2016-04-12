#pragma once
namespace FeedTheDog
{
	// Service Manager & ThreadPool
	template<typename TCorePolicy>
	class Core :
		private _BOOST noncopyable
	{
	public:
		typedef typename Core TCore;
		typedef typename TCorePolicy::TConfig TConfig;
		typedef typename TCorePolicy::TTraceSource TTraceSource;
		typedef typename TCorePolicy::template TCore<Core>::TWorkerType TWorker;
		typedef typename TCorePolicy::template TCore<Core>::TService TService;
		Core()
		{
			isStop = true;
			tmpWorkerIndex = -1;
			config = make_unique<TConfig>();
			trace = make_unique<TTraceSource>(*config);
			GetTrace()->DebugPoint("New Core");
			threadCount = config->GetThreadCount();
			assert(threadCount > 0 && threadCount <= config->GetMaxThreadCount());

			for (size_t i = 0; i < threadCount; i++)
			{
				workers.push_back(make_unique<TWorker>(this));
			}
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
			services.insert(_STD pair<const char*, shared_ptr<TService>>(svr->Name(), svr));
			GetTrace()->DebugPoint("Add Service", false, 0, svr->Name());
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
		//	// 由服务控制是否接受新连接
		//	find->Stop();
		//	GetTrace()->DebugPoint(LogMsg::AddService, false, 0, svr->Name());
		//	// 把对应服务的所有session关掉
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
			GetTrace()->DebugPoint("Core Start");
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

			// 自身线程也算在内
			workers[0]->Start();
			// join等待所有线程结束
			for each (auto& var in threads)
			{
				var->join();
			}
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
			for (size_t i = 0; i < threadCount; i++)
			{
				workers[i]->Stop();
			}
		}
		int GetWorkerCount() const
		{
			return workers.size();
		}
		// 取空闲Worker
		TWorker* SelectIdleWorker()
		{
			assert(workers.size() > 0);
			TWorker* result = NULL;
			if (isStop)
			{
				// 当core未开时用于分散此时添加的服务
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
		_STD vector<unique_ptr<TWorker>> workers;
		concurrent_unordered_map<const char*, shared_ptr<TService>> services;
		unsigned int threadCount;
	};

}  // namespace FeedTheDog
