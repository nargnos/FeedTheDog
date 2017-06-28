#include <cstdio>
#include <sys/epoll.h>
#include <utility>
#include <thread>
#include <memory>
#include <vector>
#include <valarray>
#include <cassert>
#include <iostream>
#include <string>
#include "IoService.h"
#include "TcpServer.h"
#include "Task.h"
#include "Util.h"
#include "Timer.h"
#include "FdTask.h"
using namespace std;

BlockPtr GetResponseHead()
{
	static auto str = "Echo: ";
	static BlockPtr result(new CharBlock(str, strlen(str)));
	return result;
}
BlockPtr GetResponseEnd()
{
	static auto str = "\n";
	static BlockPtr result(new CharBlock(str, strlen(str)));
	return result;
}
void ReadCb(TcpConnection& self, Buffer&& buf, Error e);
void WriteCb(TcpConnection& self, Buffer&& buf, Error e)
{

	if (!e)
	{
		return;
	}
	self.AsyncReadSome(ReadCb);
}
void ReadCb(TcpConnection& self, Buffer&& buf, Error e)
{
	if (!e)
	{
		return;
	}
	self.AsyncWrite(std::move(buf), 0, WriteCb);
}
// 未完成，各线程互相隔离，用作实验，没什么可用性

// FIX: 虚拟机有些东西不好跟踪，换实体机再弄
// FIX: 结构再弄简单点, 有些不需要的判断函数需要删除
// FIX: 某些执行路径会进行一些重复判断，需要简化
// TODO: close/connect 等连接，关于地址的部分是否直接用sockaddr_in比较好
// TODO: timer/asyncconnect/resolve
// TODO: 加一个事件触发的connection
// TODO: vs项目这边加了一些文件，附带的eclipse项目文件未更新，用eclipse载入时刷新下就好了
// TODO: buffer的写入不够方便，添加组合不够方便
// TODO: connect可能要添加一个类来处理，看看能不能平衡连接数，但是这样会导致在做转发时跨线程使用连接
// TODO: 泄露待查
// TODO: 整合timer进conn，做一个方便调用的超时机制
// TODO: 想一想Connector要怎么弄更方便
// TODO: 测试不够，补充测试代码；性能待优化

constexpr int echoTestOnce = 8999;
constexpr int echoTestDefault = 9000;
constexpr int echoTestDefault2 = 9001;

constexpr int echoTestClose = 9002;
constexpr int echoTestWithStrs = 9003;


int main()
{
	//if (daemon(0, 0) == -1)
	//{
	//	return -1;
	//}
	TRACEPOINT(LogPriority::Info)("Start test");
	std::vector<std::shared_ptr<ITcpServer>> servers;
	PostFdTaskIdle([](Detail::Loop&)
	{
		PostTimer([](const TimerCallback& t)
		{
			cout << " " << endl;
			PostTimer(t.shared_from_this(), std::chrono::milliseconds(500));
		}, std::chrono::milliseconds(500));
		PostTimer([](const TimerCallback& t)
		{
			cout << "hi" << endl;
			PostTimer(t.shared_from_this(), std::chrono::seconds(1));
		}, std::chrono::seconds(1));
	});
	// once
	servers.push_back(CreateTcpServer(MakeSockaddr(INADDR_ANY, echoTestOnce), [](TcpConnection& conn)
	{
		conn.AsyncReadSome([](TcpConnection& self, Buffer&& buf, Error e)
		{
			if (!e)
			{
				return;
			}
			self.AsyncWrite(std::move(buf), 0, [](TcpConnection& self, Buffer&& buf, Error e)
			{
				self.Close();
			});
		});
	}));
	// default
	servers.push_back(CreateTcpServer(MakeSockaddr(INADDR_ANY, echoTestDefault), [](TcpConnection& conn)
	{
		conn.AsyncReadSome(ReadCb);
	}));
	// default2
	servers.push_back(CreateTcpServer(MakeSockaddr(INADDR_ANY, echoTestDefault2), [](TcpConnection& conn)
	{
		conn.AsyncReadSome(ReadCb);
	}));
	for (auto& i : servers)
	{
		i->Join();
	}
	return 0;
}
