#include "stdafx.h"
#include "Core.h"
#include "SessionPool.h"

namespace FeedTheDog
{
	Core::Core()
	{
		isStop = true;
		isInited = false;

	}
	void Core::Init()
	{
		if (isInited)
		{
			return;
		}
		isInited = true;
		config.Load();
		auto threadCount = config.GetThreadCount();
		assert(threadCount > 0 && threadCount <= config.GetMaxThreadCount());

		for (auto i = 0; i < threadCount; i++)
		{
			workers.push_back(make_shared<TWorker>(this->shared_from_this()));
		}
	}
	Core::~Core()
	{
		Stop();
	}
	Core::TSessionPool* Core::GetIdleSessionPool()
	{
		auto result = SelectIdleWorker()->GetSessionPool();
		assert(result != NULL);
		return result;
	}

	Core::TService* Core::GetService(long long id)
	{
		if (services.count(id) > 0)
		{
			return services[id].get();
		}
		return NULL;
	}



	bool Core::AddService(const shared_ptr<TService>& svr)
	{
		if (services.count(svr->UniqueID()) > 0)
		{
			return false;
		}
		if (!svr->Init(this))
		{
			return false;
		}
		services.insert(_STD pair<long long, shared_ptr<TService>>(svr->UniqueID(), svr));
		svr->AsyncStart();
		return true;
	}
	void Core::DeleteService(const shared_ptr<TService>& svr)
	{
		GetService(svr->UniqueID())->Stop();
		mutex.lock();
		services.unsafe_erase(svr->UniqueID());
		mutex.unlock();
	}
	void Core::Start()
	{
		isStop = false;
		_STD vector<shared_ptr<_BOOST thread>> threads;
		auto tmpCount = workers.size();// -1;
		if (tmpCount > 0)
		{
			for (size_t i = 0; i < tmpCount; i++)
			{
				auto tmpThread = make_shared<_BOOST thread>(_BOOST bind(&TWorker::Start, workers[i]));

				threads.push_back(tmpThread);
			}
		}
		// 自身线程也算在内
		//workers[0]->Start();
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
		__assume(result != NULL);
		return result;
	}
}  // namespace FeedTheDog
