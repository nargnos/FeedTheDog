#pragma once

namespace FeedTheDog
{
	class WorkerBase :
		private _BOOST noncopyable
	{
	public:
		WorkerBase()
		{
			isRunning = false;
			static int wid = 0;
			id = wid++;
		}

		virtual ~WorkerBase()
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

}  // namespace FeedTheDog
