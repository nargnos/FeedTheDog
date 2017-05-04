#pragma once
#ifndef TCPACCEPTOR_H_
#define TCPACCEPTOR_H_
#include <memory>
#include <atomic>
#include <functional>
#include <array>
#include "ITcpAcceptor.h"
#include "SocketCpp.h"
#include "ObjStore.h"
#include "Logger.h"
#include "TcpProactorConnection.h"
class Worker;
class IoService;
class TcpAcceptor :
	public std::enable_shared_from_this<TcpAcceptor>,
	public IFDTask,
	//public ITask,
	public Noncopyable
{
public:
	using ConnectionPtr = TcpConnection*;
	using AcceptHandler = std::function<void(ConnectionPtr)>;
	using FaildHandler = std::function<void()>;
	virtual int FD() const;
	void Cancel();
	// ��������Ϊ void(const std::shared_ptr<TcpConnection>&)
	void SetOnAccept(AcceptHandler&& handler);
	// Ĭ�ϳ�����˳�, ���ú��ȡ��Ĭ�ϲ���
	void SetOnFaild(FaildHandler&& handler);

	~TcpAcceptor();
	static std::shared_ptr<TcpAcceptor> Create(const std::shared_ptr<IoService>& ios,
		const sockaddr_in& bind);
private:
	TcpAcceptor(const std::shared_ptr<IoService>& ios,
		const sockaddr_in& bind);
	void RegListen();
	void UnRegListen(Loop& loop);
	void DoAccept(Loop & loop, int fd);
	// ���ڶ���̱߳����ã���һ��ֻ��һ���߳��ܷ���
	virtual void DoEvent(Loop& loop, EpollOption op);
	void OnFaild();

	FaildHandler onFaild_;
	std::shared_ptr<IoService> ios_;
	TcpSocket socket_;
	AcceptHandler onAccept_;
	std::atomic_int_fast16_t count_;
	std::atomic_bool isCanceled_;
	bool needReregister_;
};

#endif // !TCPACCEPTOR_H_

