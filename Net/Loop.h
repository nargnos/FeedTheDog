
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
#include "TaskQueue.h"
#include "EpollCpp.h"

class FDTaskCtlProxy;
class RunProxy;
class ConnectionRegisterProxy;
// ֻ������ע�������¶������̰߳�ȫ�Ľӿ�
class Loop final :public Noncopyable
{
public:
	enum LoopState
	{
		Running,
		Stopping,
		Stopped
	};
	friend FDTaskCtlProxy;
	friend RunProxy;
	friend ConnectionRegisterProxy;
	Loop();
	~Loop();
	LoopState State() const;
	const std::thread::id& OwnerTid()const;
	// ���·��̰߳�ȫ
	void QueueTask(std::unique_ptr<ITask>&& ptr);
private:
	// �����ظ�ִ��
	void Start();
	void PrepareBuffers();
	// �ᵼ��δ������񱻶���
	void Stop();
	bool CheckTid() const;
	EpollCpp& Epoll();
	TaskQueue taskQueue_;
	EpollCpp epoll_;
	std::thread::id tid_;
	int loopID_;
	int stopfd_;
	int waitMs_;
	LoopState state_;
	static const int Infinite = -1;

};

#endif /* LOOP_H_ */
