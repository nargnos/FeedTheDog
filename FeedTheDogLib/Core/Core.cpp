#include "Core.h"


namespace FeedTheDog
{
	Core::Core()
	{
		config.Load();
		strand = make_unique<typename _ASIO io_service::strand>(ioService);
		work = make_unique<_ASIO io_service::work>(ioService);
		sessionPool = make_unique<TSessionPool>(this);
		worker = make_unique<TWorker>(this);
		
	}
	Core::~Core()
	{
		Stop();
	}

	_ASIO io_service & Core::GetIoService()
	{
		return ioService;
	}

	typename _ASIO io_service::strand* Core::GetStrand()
	{
		return strand.get();
	}

	void Core::AddService(const shared_ptr<TService>& svr)
	{
		services.insert(_STD pair<int, shared_ptr<TService>>(svr->UniqueID(), svr));
		GetIoService().post(_BOOST bind(&TService::Run, svr, this));
	}
	void Core::DeleteService(const shared_ptr<TService>& svr)
	{
		services.erase(svr->UniqueID());
	}
	void Core::Start()
	{
		worker->Start();
	}
	void Core::Stop()
	{
		worker->Stop();
	}
	shared_ptr<TTcpSession> Core::GetTcpSession()
	{
		return sessionPool->GetTcpSession();
	}
	shared_ptr<TUdpSession> Core::GetUdpSession()
	{
		return sessionPool->GetUdpSession();
	}
}  // namespace FeedTheDog
