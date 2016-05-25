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
		_ASIO io_service& GetIoService();

		friend class WorkerFriendProxy;
	protected:
		// 只在程序开始使用
		void Start();
		// 只在程序结束使用
		void Stop();
		_ASIO io_service ioService_;	

		int id_;
		bool isRunning_;
	};

}  // namespace FeedTheDog
