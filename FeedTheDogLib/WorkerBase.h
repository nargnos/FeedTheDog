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
		// ֻ�ڳ���ʼʹ��
		void Start();
		// ֻ�ڳ������ʹ��
		void Stop();
		_ASIO io_service ioService_;	

		int id_;
		bool isRunning_;
	};

}  // namespace FeedTheDog
