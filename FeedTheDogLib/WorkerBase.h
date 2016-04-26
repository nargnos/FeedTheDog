#pragma once

namespace FeedTheDog
{
	class WorkerBase :
		private _BOOST noncopyable
	{
	public:
		WorkerBase();

		virtual ~WorkerBase();
		int GetID() const;
		// 只在程序开始使用
		void Start();
		// 只在程序结束使用
		void Stop();
		_ASIO io_service& GetIoService();
	protected:
		_ASIO io_service ioService;
		unique_ptr<_ASIO io_service::work> work;
		int id;

		bool isRunning;
		virtual void CloseAllSessions() = 0;
	};

}  // namespace FeedTheDog
