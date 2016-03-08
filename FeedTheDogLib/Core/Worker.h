#pragma once
#include "..\Common.h"
#include "..\Config.h"
#include "..\Trait\WorkerTrait.h"
#include "..\Trait\CoreTrait.h"
namespace FeedTheDog
{
	class Worker:
		private _BOOST noncopyable
	{
	public:
		typedef Worker TWorker;
		typedef typename WorkerTrait::TService TService;
		typedef typename CoreTrait::TCore TCore;
		typedef typename WorkerTrait::TSessionPool TSessionPool;
		Worker(TCore* core);
		virtual ~Worker();
		TSessionPool* GetIdleSessionPool();
		_ASIO io_service* GetIoService();
		int GetID() const;
		void Start();
		void Stop();
		
		TCore* GetCore() const;
	private:
		unique_ptr<_ASIO io_service> ioService;
		shared_ptr<TSessionPool> sessionPool;
		unique_ptr<_ASIO io_service::work> work;
		int id;
		TCore* owner;
	};
}  // namespace FeedTheDog
