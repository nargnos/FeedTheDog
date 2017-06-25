#include "ProactorConnection.h"
#include "TlsPackage.h"
#include "SocketCpp.h"
namespace Detail
{
	ProactorConnection::ProactorConnection(Loop & loop, Socket* socketPtr) :
		storePos_(GlobalTlsPackage::Instance().TlsConnectionList.end()),
		loop_(loop),
		socketPtr_(socketPtr),
		currentEvent_(EPOLLIN | EPOLLOUT),
		isSocketRegistered_(false),
		isTaskRegistered_(false),
		isProtected_(false)
	{
	}
#ifdef NDEBUG
	ProactorConnection::~ProactorConnection() = default;
#else
	ProactorConnection::~ProactorConnection()
	{
		assert(storePos_ == GlobalTlsPackage::Instance().TlsConnectionList.end());
		assert(IsAllOpQueueEmpty());
	}
#endif // NDEBUG

	void ProactorConnection::ClearQueues(Error err)
	{
		for (auto& i : opqueue_)
		{
			ClearQueue(i, err);
		}
	}

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
		if (CurrentEvent() ^ e)
		{
			RegTask();
		}
	}
	void ProactorConnection::PostReadOp(OpPtr && op)
	{
		// assert(!(currentEvent_ & EPOLLERR));
		opqueue_[0].push(std::move(op));
	}
	void ProactorConnection::PostWriteOp(OpPtr && op)
	{
		// assert(!(currentEvent_ & EPOLLERR));
		opqueue_[1].push(std::move(op));
	}
	void ProactorConnection::Protect()
	{
		if (isProtected_)
		{
			return;
		}
		auto& tls = GlobalTlsPackage::Instance();
		auto storeEnd = tls.TlsConnectionList.end();
		assert(storePos_ == storeEnd);
		storePos_ = tls.TlsConnectionList.insert(storeEnd, shared_from_this());
		isProtected_ = true;
	}
	void ProactorConnection::Destory()
	{
		if (!isProtected_)
		{
			return;
		}
		auto& tls = GlobalTlsPackage::Instance();
		auto storeEnd = tls.TlsConnectionList.end();
		assert(storePos_ != storeEnd);
		tls.TlsConnectionList.erase(storePos_);
		storePos_ = storeEnd;
		isProtected_ = false;
	}
	bool ProactorConnection::HasError() const
	{
		return currentEvent_&EPOLLERR;
	}
	EpollOption ProactorConnection::CurrentEvent() const
	{
		return currentEvent_;
	}

	void ProactorConnection::Recycle()
	{
		CheckTid();
		assert(&loop_ == GlobalTlsPackage::Instance().TlsLoop);
		assert(storePos_ == GlobalTlsPackage::Instance().TlsConnectionList.end());
		assert(IsAllOpQueueEmpty());
		assert(!isSocketRegistered_);
		assert(!isTaskRegistered_);
		assert(!isProtected_);
		if (socketPtr_ != nullptr)
		{
			socketPtr_->Close();
			socketPtr_ = nullptr;
		}
		currentEvent_ = EPOLLIN | EPOLLOUT;
	}
	void ProactorConnection::CheckFd()
	{
		assert(socketPtr_ != nullptr);
		assert(socketPtr_->FD() != -1);
	}
	int ProactorConnection::FD() const
	{
		assert(socketPtr_ != nullptr);
		return socketPtr_->FD();
	}
	void ProactorConnection::RegSocket()
	{
		if (__glibc_likely(isSocketRegistered_))
		{
			return;
		}
		assert(!HasError());
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
	void ProactorConnection::SetNonBlocking()
	{
		assert(socketPtr_ != nullptr);
		if (UNLIKELY(!socketPtr_->SetNonBlocking()))
		{
			TRACEPOINT(LogPriority::Warning)("tcpsocket setnonblocking failed");
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
				Error err{ 0,ErrStat::None };
				tmpQueue.front()->DoOperator(err);
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
				{
					auto tmpop = std::move(tmpQueue.front());
					tmpQueue.pop();
					tmpop->DoComplete(err);
				}
				break;
				case ErrStat::Close:
				case ErrStat::Error:
					currentEvent_.Value |= EPOLLERR;
					UnRegSocket();
					ClearQueue(opqueue_[i], err);
					Destory();
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
			UnRegSocket();
			Destory();
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
	void ProactorConnection::Close()
	{
		if (socketPtr_ != nullptr)
		{
			UnRegSocket();
			socketPtr_->Close();
			socketPtr_ = nullptr;
			ClearQueues(Error{ 0,ErrStat::Close });
			Destory();
			assert(IsAllOpQueueEmpty());
		}
	}
}  // namespace Detail


