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
		// ֻ�ڳ���ʼʹ��
		void Start();
		// ֻ�ڳ������ʹ��
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
