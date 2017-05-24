#include "ProactorConnection.h"
namespace Detail
{
	ProactorConnection::ProactorConnection(Loop & loop) :
		loop_(loop),
		currentEvent_(EPOLLIN | EPOLLOUT),
		isSocketRegistered_(false),
		isTaskRegistered_(false)
	{
	}
#ifdef NDEBUG
	ProactorConnection::~ProactorConnection() = default;
#else
	ProactorConnection::~ProactorConnection()
	{
		assert(IsAllOpQueueEmpty());
	}
#endif // NDEBUG

	void ProactorConnection::RegTask()
	{
		if (__glibc_likely(isTaskRegistered_))
		{
			return;
		}
		isTaskRegistered_ = true;
		RegisterTaskAttorney::RegisterTask(loop_, shared_from_this());
	}
	void ProactorConnection::CheckAndRegTask(EpollOption e)
	{
		if (CurrentEvent()&e)
		{
			RegTask();
		}
	}
	void ProactorConnection::PostReadOp(OpPtr && op)
	{
		assert(!(currentEvent_ & EPOLLERR));
		opqueue_[0].push(std::move(op));
	}
	void ProactorConnection::PostWriteOp(OpPtr && op)
	{
		assert(!(currentEvent_ & EPOLLERR));
		opqueue_[1].push(std::move(op));
	}
	void ProactorConnection::Protect()
	{
		if (store_)
		{
			return;
		}
		store_ = shared_from_this();
	}
	void ProactorConnection::Destory()
	{
		store_ = nullptr;
	}
	bool ProactorConnection::HasError() const
	{
		return currentEvent_&EPOLLERR;
	}
	EpollOption ProactorConnection::CurrentEvent() const
	{
		return currentEvent_;
	}
	void ProactorConnection::RegSocket()
	{
		if (__glibc_likely(isSocketRegistered_))
		{
			return;
		}
		isSocketRegistered_ = true;
		FDTaskCtlAttorney::Add(loop_, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP, this);
	}
	void ProactorConnection::UnRegSocket()
	{
		if (__glibc_likely(isSocketRegistered_))
		{
			isSocketRegistered_ = false;
			FDTaskCtlAttorney::Del(loop_, this);
		}
	}
	void ProactorConnection::DoEvent(Loop & loop, EpollOption op)
	{
		assert(&loop_ == &loop);
		if (currentEvent_ == op)
		{
			return;
		}
		currentEvent_ = op;
		RegTask();
	}
	bool ProactorConnection::DoEvent(Loop & loop)
	{
		assert(&loop_ == &loop);
		assert(!IsAllOpQueueEmpty());

		static constexpr uint32_t flags[]
		{
			EPOLLIN | EPOLLRDHUP ,
			EPOLLOUT
		};

		for (size_t i = 0; i < MaxOps; i++)
		{
			if ((currentEvent_&(flags[i] | EPOLLERR | EPOLLHUP)) &&
				!opqueue_[i].empty())
			{
				auto& tmpQueue = opqueue_[i];
				auto& op = *tmpQueue.front();
				Error err{ 0,ErrStat::None };
				op.DoOperator(err);
				switch (err.State)
				{
				case ErrStat::Continue:
					break;
				case ErrStat::Pause:
					currentEvent_ = ~flags[i] & currentEvent_;
					Protect();
					RegSocket();
					break;
				case ErrStat::Success:
					// 会影响队列元素
					op.DoComplete(err);
					tmpQueue.pop();
					break;
				case ErrStat::Close:
				case ErrStat::Error:
					UnRegSocket();
					Destory();
					currentEvent_.Value |= EPOLLERR;
					ClearQueue(opqueue_[i], err);
					break;
				default:
					assert(false);
					__builtin_unreachable();
					break;
				}
			}
		}

		auto readEmpty = opqueue_[ReadOp].empty();
		auto writeEmpty = opqueue_[WriteOp].empty();
		if (((currentEvent_&EPOLLIN) && !readEmpty) ||
			((currentEvent_&EPOLLOUT) && !writeEmpty))
		{
			return false;
		}
		if (readEmpty&&writeEmpty)
		{
			Destory();
			UnRegSocket();
		}
		isTaskRegistered_ = false;
		return true;

	}
	void ProactorConnection::ClearQueue(OpQueue & q, Error err)
	{
		while (!q.empty())
		{
			q.front()->DoComplete(err);
			q.pop();
		}
	}
	bool ProactorConnection::IsAllOpQueueEmpty()
	{
		assert(MaxOps == 2);
		return opqueue_[0].empty() && opqueue_[1].empty();
	}
}  // namespace Detail


