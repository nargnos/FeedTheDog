#include "stdafx.h"
#include "Core.h"
#include "SessionPool.h"
#include "Worker.h"
namespace FeedTheDog
{
	Core::Core()
	{
		isStop = true;
		config.Load();
		GetTrace()->TracePoint(LogMsg::NewCore);
		auto threadCount = config.GetThreadCount();
		assert(threadCount > 0 && threadCount <= config.GetMaxThreadCount());

		for (auto i = 0; i < threadCount; i++)
		{
			workers.push_back(make_shared<TWorker>(this));
		}
		config.Save();
		GetTrace()->TracePoint(LogMsg::Initialized);
	}
	Core::~Core()
	{
		GetTrace()->TracePoint(LogMsg::FreeCore);
		Stop();
	}
	shared_ptr<TraceSource<Config::TEnum>>& Core::GetTrace()
	{
		return config.GetTrace();
	}
	Core::TSessionPool* Core::GetIdleSessionPool()
	{
		auto result = SelectIdleWorker()->GetSessionPool();
		assert(result != NULL);
		return result;
	}

	Core::TService* Core::GetService(const char* name)
	{
		if (services.count(name) > 0)
		{
			return services[name].get();
		}
		return NULL;
	}



	bool Core::AddService(const shared_ptr<TService>& svr)
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
		GetTrace()->TracePoint(LogMsg::AddService, false, 0, svr->Name());
		svr->AsyncStart();
		return true;
	}
	void Core::DeleteService(const shared_ptr<TService>& svr)
	{
		auto find = GetService(svr->Name());
		if (find == NULL)
		{
			return;
		}
		// TODO: 把对应服务的所有session关掉
		find->Stop();
		GetTrace()->TracePoint(LogMsg::AddService, false, 0, svr->Name());
		mutex.lock();
		services.unsafe_erase(svr->Name());
		mutex.unlock();


	}
	void Core::Start()
	{
		GetTrace()->TracePoint(LogMsg::CoreStart);
		isStop = false;
		_STD vector<shared_ptr<_BOOST thread>> threads;
		auto tmpCount = workers.size();
		if (tmpCount > 1)
		{
			for (size_t i = 1; i < tmpCount; i++)
			{
				auto tmpThread = make_shared<_BOOST thread>(_BOOST bind(&TWorker::Start, workers[i]));

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
	void Core::Stop()
	{
		if (isStop)
		{
			return;
		}
		GetTrace()->TracePoint(LogMsg::CoreStop);
		isStop = true;
		for each (auto& var in services)
		{
			var.second->Stop();
		}
		for (size_t i = 0; i < workers.size(); i++)
		{
			workers[i]->Stop();
		}
	}
	int Core::GetWorkerCount() const
	{
		return workers.size();
	}
	// TODO: 让没开始时不要往一个线程弄太多队列
	Core::TWorker* Core::SelectIdleWorker()
	{
		assert(workers.size() > 0);
		TWorker * result = workers[0].get();
		if (!isStop)
		{
			for (size_t i = 1; i < workers.size(); i++)
			{
				auto tmpWorker = workers[i].get();
				if (tmpWorker->GetSessionPool()->GetSessionCount() < result->GetSessionPool()->GetSessionCount())
				{
					result = tmpWorker;
				}
			}
		}
		assert(result != NULL);
		return result;
	}
}  // namespace FeedTheDog
