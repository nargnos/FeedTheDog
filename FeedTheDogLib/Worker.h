#pragma once
#include "Config.h"
#include "WorkerTrait.h"
#include "CoreTrait.h"
#include "SessionPool.h"
namespace FeedTheDog
{
	class Worker :
		private _BOOST noncopyable
	{
	public:
		typedef Worker TWorker;
		typedef typename WorkerTrait::TService TService;
		typedef typename CoreTrait::TCore TCore;
		typedef typename WorkerTrait::TSessionPool TSessionPool;
		Worker(const shared_ptr<TCore>& core);
		virtual ~Worker();
		TSessionPool* GetSessionPool();
		_ASIO io_service& GetIoService();
		int GetID() const;
		void Start();
		void Stop();

		shared_ptr<Worker::TCore> GetCore() const;
	private:
		_ASIO io_service ioService;
		TSessionPool sessionPool;
		unique_ptr<_ASIO io_service::work> work;
		int id;
		weak_ptr<TCore> owner;
	};
}  // namespace FeedTheDog
