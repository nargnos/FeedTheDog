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
		~Worker();
		TSessionPool* GetSessionPool();
		_ASIO io_service& GetIoService();
		int GetID() const;
		// 只在程序开始使用
		void Start();
		// 只在程序结束使用
		void Stop();

		TCore* GetCore() const;
	private:
		_ASIO io_service ioService;
		TSessionPool sessionPool;
		unique_ptr<_ASIO io_service::work> work;
		int id;
		TCore* owner;
		bool isRunning;
	};
}  // namespace FeedTheDog
