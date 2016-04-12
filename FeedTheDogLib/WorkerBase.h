#pragma once

namespace FeedTheDog
{
	class _WorkerBase :
		private _BOOST noncopyable
	{
	public:
		_WorkerBase()
		{
			isRunning = false;
			static int wid = 0;
			id = wid++;
		}

		virtual ~_WorkerBase()
		{
		}
		int GetID() const
		{
			return id;
		}
		// 只在程序开始使用
		void Start()
		{
			ioService.reset();
			work = make_unique<_ASIO io_service::work>(ioService);
			isRunning = true;
			ioService.run();
		}
		// 只在程序结束使用
		void Stop()
		{
			isRunning = false;
			CloseAllSessions();
			work.reset();
		}
		_ASIO io_service& GetIoService()
		{
			return ioService;
		}
	protected:
		_ASIO io_service ioService;
		unique_ptr<_ASIO io_service::work> work;
		int id;

		bool isRunning;
		virtual void CloseAllSessions() = 0;
	};

	template<typename TOwner>
	class WorkerBase:public _WorkerBase
	{
	public:
		typedef TOwner TOwner;
		WorkerBase(TOwner* owner)
		{
			owner_ = owner;
		}		
		TOwner* GetOwner()
		{
			return owner_;
		}	
	protected:
		TOwner* owner_;	
	};
}  // namespace FeedTheDog
