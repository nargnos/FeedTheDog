#include "stdafx.h"
#include "TraceSource.h"
#include "Core.h"
#include "Worker.h"
#include "WorkerPool.h"
#include "Config.h"
#include "IService.h"
#include "ServiceBaseImpl.h"
namespace FeedTheDog
{
	class WorkerPoolFriendProxy
	{
	public:
		friend class Core;
	private:
		static void Start(WorkerPool* pool)
		{
			pool->Join();
		}
		static void Stop(WorkerPool* pool)
		{
			pool->Stop();
		}
		WorkerPoolFriendProxy() = delete;

		~WorkerPoolFriendProxy() = delete;
	};

	Core::Core() :
		isStopped(true),
		workerPool_(make_unique<TWorkerPool>())
	{
		serviceBaseImpl_ = make_shared<ServiceBaseImpl>(*workerPool_);
	}

	Core::~Core()
	{
	}

	const shared_ptr<Core::TService>& Core::GetService(const char * name)
	{
		if (services.count(name) > 0)
		{
			return services[name];
		}
		return NULL;
	}

	bool Core::AddService(const shared_ptr<Core::TService>& svr)
	{
		if (services.count(svr->Name()) > 0)
		{
			return false;
		}
		auto thisPtr = this->shared_from_this();
		if (!svr->Init(serviceBaseImpl_))
		{
			return false;
		}
		services.insert({ svr->Name(), svr });
		svr->Start();
		return true;
	}

	void Core::Start()
	{
		isStopped = false;
		WorkerPoolFriendProxy::Start(workerPool_.get());
	}
	void Core::Stop()
	{
		// �������������Ҫִ�еĶ��������ܽ�������start������ˣ������Ҫ������new
		if (isStopped)
		{
			return;
		}
		isStopped = true;
		// �˴�ֹͣ���񲻻�ص��Ự
		for each (auto& var in services)
		{
			var.second->Stop();
		}
		// �˴���ز�ɾ���Ự
		WorkerPoolFriendProxy::Stop(workerPool_.get());
	}
	const unique_ptr<Core::TWorkerPool>& Core::GetWorkerPool() const
	{
		return workerPool_;
	}
}  // namespace FeedTheDog
