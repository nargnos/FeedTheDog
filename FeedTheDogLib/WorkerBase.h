#pragma once
#include "Config.h"
#include "CoreTrait.h"
namespace FeedTheDog
{
	class WorkerBase :
		private _BOOST noncopyable
	{
	public:
		typedef typename CoreTrait::TCore TCore;
		WorkerBase(TCore* core);

		~WorkerBase();
		int GetID() const;
		TCore* GetCore() const;
		// ֻ�ڳ���ʼʹ��
		void Start();
		// ֻ�ڳ������ʹ��
		void Stop();
		_ASIO io_service& GetIoService();
	protected:
		_ASIO io_service ioService;
		unique_ptr<_ASIO io_service::work> work;
		int id;
		TCore* owner;
		bool isRunning;
		_ASIO io_service::strand strand;
		virtual void CloseAllSessions() = 0;
	};
}  // namespace FeedTheDog
