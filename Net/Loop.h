
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

class FDTaskCtlProxy;
class RunProxy;
class ConnectionRegisterProxy;
// 只对自身注册的任务暴露自身非线程安全的接口
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
	// 以下非线程安全
	void RegisterTask(std::unique_ptr<ITask>&& ptr);
private:
	// 不可重复执行
	void Start();
	void PrepareBuffers();
	// 会导致未完成任务被丢弃
	void Stop();
	bool CheckTid() const;
	EpollCpp& Epoll();

	TaskList taskList_;
	EpollCpp epoll_;
	std::thread::id tid_;
	int loopID_;
	int stopfd_;
	int waitMs_;
	LoopState state_;
	static const int Infinite = -1;

};

#endif /* LOOP_H_ */
