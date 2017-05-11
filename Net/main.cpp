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
	// TODO: 使用不够方便
	// 重复分配
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

	// 跨线
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
			// 因为析构时未创建pool
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
	// 改大小
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
	// TODO: 错误提示需要更详尽
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
	// TODO: 这个结构不好
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
// 未完成，使用这个结构是因为想只用epoll的线程安全机制和atomic实现整个结构线程安全；用作实验，没什么可用性

// FIX: 虚拟机有些东西不好跟踪，换实体机再弄
// FIX: 结构再弄简单点, 有些不需要的判断函数需要删除

// TODO: close/connect 等连接，关于地址的部分是否直接用sockaddr_in比较好
// TODO: timer/asyncconnect/resolve
// TODO: 加一个事件触发的connection
// TODO: vs项目这边加了一些文件，附带的eclipse项目文件未更新，用eclipse载入时刷新下就好了
// TODO: buffer的写入不够方便，添加组合不够方便
// TODO: connect可能要添加一个类来处理，看看能不能平衡连接数，但是这样会导致在做转发时跨线程使用连接
// TODO: 需要添加明确的禁止跨线程使用conn
// TODO: 编译时对传入类型检查，否则提示不准确提升查错难度
// TODO: 错误信息不够详细
// TODO: 泄露待查



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
	//			std::cout << "连接-写 成功" << std::endl;
	//		});
	//	}
	//	else
	//	{
	//		std::cout << "连接失败: " <<std::endl;
	//	}
	//});

	ios->Wait();
	return 0;
}
