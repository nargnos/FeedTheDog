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
// 未完成
// FIX: 虚拟机有些东西不好跟踪，换实体机再弄
// FIX: 结构再弄简单点, 有些不需要的判断函数需要删除
// FIX: 为什么交换线程亲和性会导致性能下降，虚拟机perf跟踪时在某halt函数消耗，需要换实体机跟踪看看
// 把模拟proactor的跟connection分离（弄得跟iocp一样就好替换）
// TODO: close/connect 等连接，关于地址的部分是否直接用sockaddr_in比较好
// TODO: timer/asyncconnect/resolve
// TODO: 加一个事件触发的connection
// TODO: vs项目这边加了一些文件，附带的eclipse项目文件未更新，用eclipse载入时刷新下就好了
void TestBuffer()
{
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
	});
	ios->Wait();
	return 0;
}
