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
		Worker(TCore* core);
		virtual ~Worker();
		TSessionPool* GetSessionPool();
		_ASIO io_service& GetIoService();
		int GetID() const;
		void Start();
		void Stop();

		Worker::TCore* GetCore() const;
	private:
		_ASIO io_service ioService;
		TSessionPool sessionPool;
		unique_ptr<_ASIO io_service::work> work;
		int id;
		TCore* owner;
		bool isRunning;
	};
}  // namespace FeedTheDog
