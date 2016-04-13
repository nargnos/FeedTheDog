#pragma once
#include "Session.h"
#include "Owner.h"
namespace FeedTheDog
{
	template<typename TProtocol,
		typename TOwner,
		typename TSessionPoolPolicy,
		typename TMemoryPoolPolicy,
		typename TSessionStoragePolicy>
	class SessionPool :
		public _BOOST noncopyable,
		public Owner<TOwner>
	{
	public:
		typedef typename TSessionPoolPolicy::template TSession<TProtocol, SessionPool>::TSessionType TSession;

		typedef typename TMemoryPoolPolicy::template MemoryPool<TSession> TMemoryPoolPolicy;
		typedef typename TMemoryPoolPolicy::TPoolPtr TPoolPtr;

		typedef typename TSessionStoragePolicy::template SessionStorage<TSession> TStorage;
		typedef typename TStorage::TStoragePtr TStoragePtr;

		typedef typename TStorage::TStorageIterator TStorageIterator;
		typedef typename TStorage::TStorageValue TStorageValue;

		typedef typename TProtocol::socket TSocket;
		typedef typename TProtocol::resolver TResolver;

		SessionPool(TOwner* owner, _ASIO io_service& io) :
			ios(io),
			count(0),
			sessionPool(_STD move(TMemoryPoolPolicy::Create(alloc))),
			resolver(io),
			sessionStorage(TStorage::Create()),
			Owner(owner)
		{
			//corePtr->GetTrace()->DebugPoint(LogMsg::NewSessionPool);
			isDestruct = false;
		}

		~SessionPool()
		{
			//corePtr->GetTrace()->DebugPoint(LogMsg::FreeSessionPool);
			isDestruct = true;
			if (count > 0)
			{
				CloseAll();
			}
			assert(count == 0);
		}
		// ���߳�ͬʱnew�����һ�����ʳ����߳�����
		shared_ptr<TSession> __fastcall NewSession()
		{
			//corePtr->GetTrace()->DebugPoint(LogMsg::NewSession);
			++count;
			// FIX: ����ʲô����ķ������Ի�
			// ����session
			auto result = TMemoryPoolPolicy::Malloc(sessionPool, this, &ios);

			// �ӿ췵���ٶȣ�������eraseִ��ʱ����Ի���insertִ��֮��			
			ios.post([&, result]() {result->insertPosition = TStorage::Insert(sessionStorage, result); });
			return shared_ptr<TSession>(result, [this](TSession* ptr) {Free(ptr); });
		}

		unsigned int GetSessionCount() const
		{
			return count;
		}
		// ��������ʱ��ֹͣǰ����
		void CloseAll()
		{
			// ֻ�ص����ӣ�ɾ������������ָ�봦��			
			//corePtr->GetTrace()->DebugPoint(LogMsg::CloseAllSocket);
			if (isDestruct)
			{
				AsyncCloseAll();
				return;
			}
			ios.post(_BOOST bind(&SessionPool::AsyncCloseAll, this));
		}
		
		TResolver& GetResolver()
		{
			return resolver;
		}
		void __PreDestruct()
		{
			isDestruct = true;
		}
	private:
		
		//_ASIO strand strand;
		// ��ʾ�Ƿ�������״̬
		bool isDestruct;

		//TCore* corePtr;
		_ASIO io_service& ios;
		_BOOST atomic<unsigned int> count;

		TResolver resolver;
		_BOOST system::error_code ignore;

		_STD allocator<TSession> alloc;
		// FIX:��(��freelock���ҵ��ģ��Ƿ���ô�����)
		TPoolPtr sessionPool;
		// ������������ָ��洢

		TStoragePtr sessionStorage;

		void AsyncCloseAll()
		{
			for each (auto& var in *sessionStorage)
			{
				var->close(ignore);
			}
		}

		void FreeSession(TSession* ptr)
		{
			//corePtr->GetTrace()->DebugPoint(LogMsg::FreeSession);

			// ����ʱ��erase��������ʱ��
			if (!isDestruct)
			{
				TStorage::Remove(sessionStorage, ptr->insertPosition);
			}

			// FIX: cpu
			// �����������ʹ��lockfree���ܻ������ٶ�
			TMemoryPoolPolicy::Free(sessionPool, ptr);
			--count;
		}

		void Free(TSession* ptr)
		{
			if (isDestruct)
			{
				// ioservice����free֮ǰֹͣ�����Բ���post
				FreeSession(ptr);
				return;
			}
			// ����dispatch����ʱ����λ�ò�һ���Ѿ�����
			ios.post([this, ptr]() {FreeSession(ptr); });
		}
	};

}  // namespace FeedTheDog