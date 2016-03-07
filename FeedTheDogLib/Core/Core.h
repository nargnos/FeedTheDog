#pragma once
#include "..\Common.h"
#include "..\Config.h"
#include "..\Trait\Trait.h"
#include "..\Trait\CoreTrait.h"
#include "..\Interface\IService.h"
#include "Worker.h"
#include "..\Pool\SessionPool.h"
namespace FeedTheDog
{
	class Core
	{
	public:
		typedef typename Core TCore;
		typedef typename CoreTrait::TWorker TWorker;
		typedef typename CoreTrait::TService TService;
		typedef typename CoreTrait::TMemoryPool TMemoryPool;
		typedef typename CoreTrait::TSessionPool TSessionPool;
		Core();
		virtual ~Core();
		_ASIO io_service& GetIoService();
		typename _ASIO io_service::strand* GetStrand();
		void AddService(const shared_ptr<TService>&);
		void DeleteService(const shared_ptr<TService>&);
		void Start();
		void Stop();
		shared_ptr<TTcpSession> GetTcpSession();
		shared_ptr<TUdpSession> GetUdpSession();
	private:
		_ASIO io_service ioService;
		Config config;

		unique_ptr<typename _ASIO io_service::strand> strand;		
		unique_ptr<TWorker> worker;
		unique_ptr<_ASIO io_service::work> work;

		unique_ptr<TSessionPool> sessionPool;

		_STD unordered_map<int, shared_ptr<TService>> services;
	};

}  // namespace FeedTheDog
