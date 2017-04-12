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

// FIX: �������Щ�������ø��٣���ʵ�����Ū
// FIX: �ṹ��Ū�򵥵�,
// FIX: Ϊʲô�����߳��׺��Իᵼ�������½��������perf����ʱ��ĳhalt�������ģ���Ҫ��ʵ������ٿ���
// ��ģ��proactor�ĸ�connection���루Ū�ø�iocpһ���ͺ��滻��
// TODO: timer/asyncconnect/resolve
// TODO: ��һ���¼�������connection
// TODO: vs��Ŀ��߼���һЩ�ļ���������eclipse��Ŀ�ļ�δ���£���eclipse����ʱˢ���¾ͺ���
// TODO: �ڴ�bufferʱ��dbgʱֻ�����ݿ���mprotect���������Ա��ʱ��ʾ��С��д���⣬��������Ѱ��������ṹ�ķ�ʽ����Ϊ��
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
