
#include "Loop.h"
#include <cassert>
#include <unistd.h>
#include <sys/eventfd.h>
#include <stdlib.h>
#include <type_traits>
#include <errno.h>
#include "EpollCpp.h"
#include "Logger.h"
#include "Buffer.h"
namespace Detail
{
	Loop::Loop() :
		EventTaskBase(1, EventFdFlags::CloseOnExec),
		state_(LoopState::Stopped)
	{
		static int ID = 0;
		loopID_ = ID++;
	}

	Loop::~Loop()
	{
		assert(state_ == LoopState::Stopped);
	}


	Loop::LoopState Loop::State() const
	{
		return state_;
	}

	std::thread::id Loop::OwnerTid() const
	{
		return tid_;
	}

	void Loop::Start()
	{
		assert(state_ == LoopState::Stopped);
		if (tid_ == std::thread::id())
		{
			tid_ = std::this_thread::get_id();
		}
		assert(tid_ == std::this_thread::get_id());

		TRACEPOINT(LogPriority::Info)("Loop Start (id: %d)", loopID_);

		state_ = LoopState::Running;
		DoLoop();
		TRACEPOINT(LogPriority::Info)("Loop Stop (id: %d)", loopID_);
		state_ = LoopState::Stopped;
		tid_ = std::thread::id();
		// �������������У�������ѭ����������
		// ERROR: ���ﲻ�ԣ�ĳЩ������������ִ���������ģ��������ͻ�ʹ����ʧ��
		// ��ʵ����Ҫstop���Ƿ�Ӧ��ȥ��
		taskList_.Clear();
	}

	void Loop::DoLoop()
	{
		std::aligned_storage_t<sizeof(EpollCpp::Events), 64> ev;
		EpollCpp::Events& events = reinterpret_cast<EpollCpp::Events&>(ev);
		int waitMs;
		try
		{
			while (true)
			{
				// �������Ķ�д�����ֶν��У����⼢��
				// ��д��EAGAIN����
				if (__glibc_likely(!taskList_.DoOnce(*this)))
				{
					waitMs = 0;
				}
				else
				{
					waitMs = Infinite;
				}
				auto nevent = Epoll().Wait(events, waitMs);

				if (__glibc_likely(nevent == 0))
				{
					continue;
				}
				auto it = events.cbegin();
				auto end = it + nevent;
				for (; it != end; ++it)
				{
					auto& item = *it;
					auto ptr = reinterpret_cast<IFDTask*>(item.data.ptr);
					assert(ptr != nullptr);
					auto ev = static_cast<EpollOption>(item.events);

					TRACEPOINT(LogPriority::Debug)(
						"FD %d Event(Loop: %d): %s%s%s%s%s%s",
						loopID_,
						ptr->FD(),
						ev.Flags.In ? "In " : "",
						ev.Flags.Out ? "Out " : "",
						ev.Flags.Pri ? "Pri " : "",
						ev.Flags.RdHup ? "RdHup " : "",
						ev.Flags.Hup ? "Hup " : "",
						ev.Flags.Err ? "Err " : "");
					ptr->DoEvent(*this, ev);
				}
			}
		}
		catch (...)
		{
			// ֹͣ
		}
	}

	void Loop::PrepareBuffers()
	{
		size_t size = 1024;
		std::vector<BlockPool::Ptr> tmp;
		tmp.reserve(size);
		for (size_t i = 0; i < size; i++)
		{
			tmp.push_back(Buffer::BufferPool().New());
		}
	}

	void Loop::Stop()
	{
		assert(state_ == LoopState::Running);
		Epoll().Add(FD(), EPOLLIN | EPOLLONESHOT, this);
		state_ = LoopState::Stopping;
	}

	void Loop::RegisterTask(std::shared_ptr<ITask>&& ptr)
	{
		taskList_.Register(std::move(ptr));
	}

	int Loop::TaskCount() const
	{
		return taskList_.Count();
	}

	bool Loop::CheckTid() const
	{
		return tid_ == std::this_thread::get_id();
	}

	EpollCpp & Loop::Epoll()
	{
		return epoll_;
	}

	void Loop::DoEvent(Loop & loop, EpollOption op)
	{
		throw nullptr;
	}
}  // namespace Detail
