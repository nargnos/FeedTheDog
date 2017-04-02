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
class IoService;
class TcpConnection;
class TcpAcceptor :
	public std::enable_shared_from_this<TcpAcceptor>,
	public StorePos<ITcpAcceptor>,
	public ITcpAcceptor,
	public Noncopyable
{
public:
	using ConnectionPtr = TcpConnection*;
	using AcceptHandler = std::function<void(ConnectionPtr)>;
	using FaildHandler = std::function<void()>;
	virtual int FD() const;
	void Cancel();
	// 函数声明为 void(const std::shared_ptr<TcpConnection>&)
	void SetOnAccept(AcceptHandler&& handler);
	// 默认出错就退出, 设置后会取消默认操作
	void SetOnFaild(FaildHandler&& handler);
	
	~TcpAcceptor();
	static std::shared_ptr<TcpAcceptor> Create(const std::shared_ptr<IoService>& ios,
		const sockaddr_in& bind);
private:
	using Store = ObjStore<ITcpAcceptor>;
	TcpAcceptor(const std::shared_ptr<IoService>& ios,
		const sockaddr_in& bind);
	// 会在多个线程被调用
	virtual void DoAccept(Loop& loop, int fd);
	virtual void DoEvent(Loop& loop, EpollOption op);

	void OnFaild();
	std::array<int, SOMAXCONN> accepts_;
	FaildHandler onFaild_;
	AcceptHandler onAccept_;
	std::shared_ptr<IoService> ios_;
	TcpSocket socket_;
	std::atomic_flag isCanceled_;
};

#endif // !TCPACCEPTOR_H_

