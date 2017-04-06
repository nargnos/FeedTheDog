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
#include "TcpConnection.h"

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

// FIX: 虚拟机有些东西不好跟踪，换实体机再弄
// FIX: 结构再弄简单点,
// 把模拟proactor的跟connection分离（弄得跟iocp一样就好替换）
// TODO: timer/asyncconnect/resolve
// TODO: 加一个事件触发的connection
int main()
{
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
