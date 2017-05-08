#include <cstdio>
#include <sys/epoll.h>
#include <utility>
#include <thread>
#include <memory>
#include <vector>
#include <valarray>
//#include 
#include <cassert>
#include <iostream>
#include "IoService.h"
#include "TcpAcceptor.h"
#include "Task.h"


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
void ReadCb(TcpConnection* self, Buffer&& buf, Error e);
void WriteCb(TcpConnection* self, Buffer&& buf, Error e)
{

	if (e != Error::Success)
	{
		return;
	}
	//buf.PopBack();
	//buf.PopFront();
	buf.StretchTo();
	self->AsyncReadSome(std::move(buf), 0, ReadCb);
}
void ReadCb(TcpConnection* self, Buffer&& buf, Error e)
{
	if (e != Error::Success)
	{
		return;
	}
	//buf.PushFront(GetResponseHead());
	//buf.PushBack(GetResponseEnd());

	self->AsyncWrite(std::move(buf), 0, WriteCb);

}
// 未完成，使用这个结构是因为想只用epoll的线程安全机制和atomic实现整个结构线程安全；用作实验，没什么可用性

// FIX: 虚拟机有些东西不好跟踪，换实体机再弄
// FIX: 结构再弄简单点, 有些不需要的判断函数需要删除

// TODO: close/connect 等连接，关于地址的部分是否直接用sockaddr_in比较好
// TODO: timer/asyncconnect/resolve
// TODO: 加一个事件触发的connection
// TODO: vs项目这边加了一些文件，附带的eclipse项目文件未更新，用eclipse载入时刷新下就好了
// vbox 的cpu0 cpu1速度有大幅差距，平均被cpu1拉得太多了，要上实体机测试才行
// TODO: buffer的写入不够方便，添加组合不够方便
// TODO: connect可能要添加一个类来处理，看看能不能平衡连接数，但是这样会导致在做转发时跨线程使用连接
// TODO: 需要添加明确的禁止跨线程使用conn

void TestBuffer()
{
	{
		auto b = Buffer::BufferPool().New(0);
	}
	int loop = 100000;
	for (int i = 0; i < loop; i++)
	{
		Buffer buf(0);
		buf.Resize(100);
		buf.StretchTo(0);
	}
}
void TestTask()
{
	int loop = 100000;
	TaskList t;
	Loop* ignore = nullptr;
	for (int i = 0; i < loop; i++)
	{
		t.Register(MakeTask([](Loop&) {return true; }));
		t.DoOnce(*ignore);
	}
}
int main()
{
	// TestBuffer();
	// TestTask();
	// return 0;

	TRACEPOINT(LogPriority::Info)("Echo test");
	auto ios = IoService::Instance();
	auto act = TcpAcceptor::Create(ios, MakeSockaddr(INADDR_ANY, 8989));
	act->SetOnAccept([](TcpAcceptor::ConnectionPtr conn) {

		Buffer buf(0);
		conn->AsyncReadSome(std::move(buf), 0, ReadCb);
		/*auto clo = conn->Clone();
		clo->AsyncConnect(MakeSockaddr("127.0.0.1", 7777), [](TcpConnection* c, Error e)
		{
			if (e == Error::Success)
			{
				Buffer b;
				auto& x = b.EmplaceBack(0);
				std::string str("hi");
				copy(str.begin(), str.end(), x->begin());
				x->resize(str.length());
				c->AsyncWrite(std::move(b), 0, WriteCb);
			}

		});*/
	});

	TcpConnection::Create().AsyncConnect(MakeSockaddr("127.0.0.1", 7777), [](TcpConnection* c, Error e)
	{
		if (e == Error::Success)
		{
			auto str = "hello world";
			auto block = std::make_shared<CharBlock>(str, strlen(str));
			Buffer b;
			b.PushBack(block);
			c->AsyncWrite(std::move(b), 0, [](TcpConnection* c, Buffer&&, Error e)
			{
				std::cout << "连接-写 成功" << std::endl;
			});
		}
		else
		{
			std::cout << "连接失败" << std::endl;
		}
	});

	ios->Wait();
	return 0;
}
