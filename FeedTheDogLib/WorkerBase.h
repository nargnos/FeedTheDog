#pragma once

namespace FeedTheDog
{
	
	template<typename TOwner>
	class WorkerBase :
		private _BOOST noncopyable
	{
	public:
		typedef TOwner TOwner;
		WorkerBase(TOwner* owner)
		{
			isRunning = false;
			static int wid = 0;
			id = wid++;
			owner_ = owner;
			//owner->GetTrace()->DebugPoint(LogMsg::NewWorker, true, id);
		}

		~WorkerBase()
		{
			//owner->GetTrace()->DebugPoint(LogMsg::FreeWorker, true, id);
		}
		int GetID() const
		{
			return id;
		}
		TOwner* GetOwner()
		{
			return owner_;
		}
		// 只在程序开始使用
		void Start()
		{
			ioService.reset();
			work = make_unique<_ASIO io_service::work>(ioService);
			isRunning = true;
			//owner->GetTrace()->DebugPoint(LogMsg::StartWorker, true, id);
			ioService.run();
		}
		// 只在程序结束使用
		void Stop()
		{
			isRunning = false;
			CloseAllSessions();
			work.reset();
			//ioService.stop();

		//	owner->GetTrace()->DebugPoint(LogMsg::StopWorker, true, id);
		}
		_ASIO io_service& GetIoService()
		{
			return ioService;
		}
	protected:
		_ASIO io_service ioService;
		unique_ptr<_ASIO io_service::work> work;
		int id;
		TOwner* owner_;
		bool isRunning;
		virtual void CloseAllSessions() = 0;
	};
}  // namespace FeedTheDog
