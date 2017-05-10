
#ifndef LOOP_H_
#define LOOP_H_
#include <memory>
#include <thread>
#include <type_traits>
#include <list>
#include <functional>
#include "Noncopyable.h"
#include "EpollOption.h"
#include "IFDTask.h"
#include "ITask.h"
#include "TaskList.h"
#include "EpollCpp.h"
#include "EventTaskBase.h"
namespace Detail
{
	class FDTaskCtlAttorney;
	class RunAttorney;
	class RegisterTaskAttorney;
	// ֻ������ע�������¶������̰߳�ȫ�Ľӿ�
	class Loop final :
		public EventTaskBase
	{
	public:
		enum LoopState:char
		{
			Running,
			Stopping,
			Stopped
		};
		friend FDTaskCtlAttorney;
		friend RunAttorney;
		friend RegisterTaskAttorney;
		Loop();
		~Loop();
		LoopState State() const;
		std::thread::id OwnerTid()const;
		// ȡ����������
		int TaskCount() const;
	private:
		virtual void DoEvent(Loop & loop, EpollOption op) override;

		// ���̰߳�ȫ
		void RegisterTask(std::shared_ptr<ITask>&& ptr);
		// �����ظ�ִ��
		void Start();
		void DoLoop();
		void PrepareBuffers();
		// �ᵼ��δ������񱻶���
		void Stop();
		bool CheckTid() const;
		EpollCpp& Epoll();

		TaskList taskList_;
		EpollCpp epoll_;
		std::thread::id tid_;
		int loopID_;
		LoopState state_;
		static const int Infinite = -1;
	};
}  // namespace Detail

#endif /* LOOP_H_ */
