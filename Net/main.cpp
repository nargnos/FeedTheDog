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
#define _STD std::
#define TestAssert(c) if(!(c)) _STD cout << "Error: "<<__LINE__ << _STD endl;


void TestBuffer()
{
	// TODO: ʹ�ò�������
	// �ظ�����
	{
		void* ptr = nullptr;
		{
			auto buffA = Buffer::BufferPool().New(0);
			buffA->resize(1);
			ptr = buffA->begin();
		}
		auto buffB = Buffer::BufferPool().New(4);
		auto buffANew = Buffer::BufferPool().New();
		TestAssert(buffB->begin() != ptr);
		TestAssert(buffANew->begin() == ptr);
		{
			int loop = 10;
			for (int i = 0; i < loop; i++)
			{
				Buffer buf(0);
				buf.Resize(100);
				buf.StretchTo(0);
			}
		}
	}

	// ����
	{
		auto buffA = Buffer::BufferPool().New();
		auto buffB = Buffer::BufferPool().New();

		void* ptrA = buffA->begin();
		void* ptrB = buffB->begin();

		thread t([buffA = move(buffA), buffB = move(buffB), ptrA, ptrB]() mutable
		{
			TestAssert(buffA->size() > 0);
			buffA = nullptr;
			TestAssert(!buffA);
			auto buffNew = Buffer::BufferPool().New();
			// ��Ϊ����ʱδ����pool
			TestAssert(ptrA != buffNew->begin());
			buffB = nullptr;
			auto buffNew2 = Buffer::BufferPool().New();
			TestAssert(ptrB == buffNew2->begin());
		});
		t.join();
		auto buffC = Buffer::BufferPool().New();
		TestAssert(ptrB != buffC->begin());
		TestAssert(ptrA != buffC->begin());

	}
	// �Ĵ�С
	{

		Buffer buff(4);
		buff.EmplaceBack(0)->resize(4);
		TestAssert(!buff.HasReadOnlyBlock());
		TestAssert(buff.Size() == 8);
		buff.StretchTo(0);
	}
}

void TestProgressRecord()
{
	// TODO: ������ʾ��Ҫ���꾡
	using namespace Detail;
	struct Ignore
	{
		bool I(ProgressRecord<Ignore>&, TransferProgress&)
		{
			return true;
		}
	};
	Ignore ignore;
	int flag = 0;
	// TODO: ����ṹ����
	std::unique_ptr<Detail::ProgressRecord<Ignore>> pr =
		Detail::MakeRecord<Ignore>([&](auto, auto) {++flag; return true; });
	pr->Complete(ignore, Detail::Error::Success);
	pr->Do(ignore, &Ignore::I);
	TestAssert(flag == 2);
}
void TestTask()
{
	int loop = 100000;
	Detail::TaskList t;
	Detail::Loop* ignore = nullptr;
	int count = 0;
	for (int i = 0; i < loop; i++)
	{
		t.Register(Detail::MakeTask([&](Detail::Loop&) {++count; return true; }));
	}
	for (int i = 0; i < loop; i++)
	{
		t.DoOnce(*ignore);
	}
	TestAssert(count == loop);
}
void TestCpuSpeed()
{
	size_t core = thread::hardware_concurrency();
	if (core == 0)
	{
		core = 1;
	}
	vector<clock_t> rec;

	size_t loop = 5000000000;
	for (size_t i = 0; i < core; i++)
	{
		SetAffinity(pthread_self(), i);
		sleep(1);

		volatile size_t sum = 0;
		auto t = clock();
		for (size_t i = 0; i < loop; i++)
		{
			sum += i;
		}
		t = clock() - t;
		rec.push_back(t);
	}

	int i = 0;
	for (auto& item : rec)
	{
		std::cout << "core" << i++ << " " << (double)item / CLOCKS_PER_SEC << std::endl;
	}
}



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



int main()
{
	//TestProgressRecord();
	// TestCpuSpeed();
	//TestBuffer();
	//TestTask();
	//return 0;

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
