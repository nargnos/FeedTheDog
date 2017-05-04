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
// δ���
// FIX: �������Щ�������ø��٣���ʵ�����Ū
// FIX: �ṹ��Ū�򵥵�, ��Щ����Ҫ���жϺ�����Ҫɾ��
// FIX: Ϊʲô�����߳��׺��Իᵼ�������½��������perf����ʱ��ĳhalt�������ģ���Ҫ��ʵ������ٿ���
// ��ģ��proactor�ĸ�connection���루Ū�ø�iocpһ���ͺ��滻��
// TODO: close/connect �����ӣ����ڵ�ַ�Ĳ����Ƿ�ֱ����sockaddr_in�ȽϺ�
// TODO: timer/asyncconnect/resolve
// TODO: ��һ���¼�������connection
// TODO: vs��Ŀ��߼���һЩ�ļ���������eclipse��Ŀ�ļ�δ���£���eclipse����ʱˢ���¾ͺ���
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
