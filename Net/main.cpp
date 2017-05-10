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
#include "TcpAcceptor.h"
#include "Task.h"
#include "Util.h"

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

	if (e != Error::Success)
	{
		return;
	}
	//buf.PopBack();
	//buf.PopFront();
	buf.StretchTo();
	self.AsyncReadSome(std::move(buf), 0, ReadCb);
}
void ReadCb(TcpConnection& self, Buffer&& buf, Error e)
{
	if (e != Error::Success)
	{
		return;
	}
	//buf.PushFront(GetResponseHead());
	//buf.PushBack(GetResponseEnd());

	self.AsyncWrite(std::move(buf), 0, WriteCb);

}
// δ��ɣ�ʹ������ṹ����Ϊ��ֻ��epoll���̰߳�ȫ���ƺ�atomicʵ�������ṹ�̰߳�ȫ������ʵ�飬ûʲô������

// FIX: �������Щ�������ø��٣���ʵ�����Ū
// FIX: �ṹ��Ū�򵥵�, ��Щ����Ҫ���жϺ�����Ҫɾ��

// TODO: close/connect �����ӣ����ڵ�ַ�Ĳ����Ƿ�ֱ����sockaddr_in�ȽϺ�
// TODO: timer/asyncconnect/resolve
// TODO: ��һ���¼�������connection
// TODO: vs��Ŀ��߼���һЩ�ļ���������eclipse��Ŀ�ļ�δ���£���eclipse����ʱˢ���¾ͺ���
// TODO: buffer��д�벻�����㣬�����ϲ�������
// TODO: connect����Ҫ���һ���������������ܲ���ƽ�������������������ᵼ������ת��ʱ���߳�ʹ������
// TODO: ��Ҫ�����ȷ�Ľ�ֹ���߳�ʹ��conn
// TODO: ����ʱ�Դ������ͼ�飬������ʾ��׼ȷ��������Ѷ�
// TODO: ������Ϣ������ϸ
// TODO: й¶����
void TestBuffer()
{
	{
		auto b = Buffer::BufferPool().New(0);
	}
	int loop = 10;
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
	Detail::TaskList t;
	Detail::Loop* ignore = nullptr;
	for (int i = 0; i < loop; i++)
	{
		t.Register(Detail::MakeTask([](Detail::Loop&) {return true; }));
		t.DoOnce(*ignore);
	}
}
void TestCpuSpeed()
{
	SetAffinity(pthread_self(), 0);
	sleep(1);
	size_t loop = 5000000000;
	volatile size_t sum = 0;
	double t1 = clock();
	for (size_t i = 0; i < loop; i++)
	{
		sum += i;
	}
	t1 = clock() - t1;

	SetAffinity(pthread_self(), 1);
	sleep(1);
	sum = 0;
	double t2 = clock();
	for (size_t i = 0; i < loop; i++)
	{
		sum += i;
	}
	t2 = clock() - t2;
	std::cout << t1 / CLOCKS_PER_SEC << "-" << t2 / CLOCKS_PER_SEC << std::endl;

}

int main()
{
	// TestCpuSpeed();
	// TestBuffer();
	// TestTask();
	// return 0;

	TRACEPOINT(LogPriority::Info)("Echo test");
	auto ios = IoService::Instance();
	auto act = TcpAcceptor::Create(MakeSockaddr(INADDR_ANY, 8989));
	act->SetOnAccept([](TcpAcceptor::ConnectionPtr conn) {

		Buffer buf(0);
		conn->AsyncReadSome(std::move(buf), 0, ReadCb);
		/*auto clo = conn->Clone();
		clo->AsyncConnect(MakeSockaddr("127.0.0.1", 7777), [](TcpConnection& c, Error e)
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

	//TcpConnection::Create().AsyncConnect(MakeSockaddr("127.0.0.1", 7777), [](TcpConnection& c, Error e)
	//{
	//	if (e == Error::Success)
	//	{
	//		auto block = std::make_shared<StringBlock>("hello world");
	//		Buffer b;
	//		b.PushBack(std::move(block));
	//		c.AsyncWrite(std::move(b), 0, [](TcpConnection& c, Buffer&&, Error e)
	//		{
	//			std::cout << "����-д �ɹ�" << std::endl;
	//		});
	//	}
	//	else
	//	{
	//		std::cout << "����ʧ��: " <<std::endl;
	//	}
	//});

	ios->Wait();
	return 0;
}
