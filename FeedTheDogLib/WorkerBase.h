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
		_ASIO io_service& FASTCALL GetIoService();
		const _ASIO io_service& FASTCALL GetIoService() const;
		unsigned int FASTCALL GetSessionCount() const;
	protected:
		ALIGN _STD atomic<size_t> counter_;
		ALIGN _ASIO io_service ioService_;
		unique_ptr<_ASIO io_service::work> work_;
		
		int id_;
		bool isRunning_;
	};

}  // namespace FeedTheDog
