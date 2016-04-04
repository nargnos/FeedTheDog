#pragma once
#include "SessionPoolTrait.h"
#include "CoreTrait.h"
#include "ISessionPool.h"
#include "Core.h"
#include "Session.h"
namespace FeedTheDog
{
	template<typename TProtocol>
	class SessionPool :
		private _BOOST noncopyable
	{
	public:
		typedef typename SessionPool<TProtocol> TSelf;
		typedef typename SessionPoolTrait::TSession<TProtocol>::type TSession;
		typedef typename SessionPoolTrait::TPool<TProtocol>::type TPool;
		typedef typename SessionPoolTrait::TSessionStorage<TProtocol>::type TStorage;
		typedef typename TStorage::iterator TStorageIterator;
		typedef typename TStorage::value_type TStorageValue;
		typedef typename CoreTrait::TCore TCore;
		typedef typename CoreTrait::TWorker TWorker;

		typedef typename TProtocol::socket TSocket;

		typedef typename TProtocol::resolver TResolver;

		SessionPool(TCore* core, TWorker* worker, _ASIO io_service& io) :
			corePtr(core),
			ios(io),
			count(0),
			sessionPool(alloc, SessionPoolTrait::PoolDefaultSize),
			worker_(worker),
			resolver(io)
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::NewSessionPool);
			isDestruct = false;
		}

		~SessionPool()
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::FreeSessionPool);
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
			corePtr->GetTrace()->DebugPoint(LogMsg::NewSession);
			++count;
			// FIX: ����ʲô����ķ������Ի�
			// ����session
			TSession* result = sessionPool.construct<true, false>(worker_);

			// �ӿ췵���ٶȣ�������eraseִ��ʱ����Ի���insertִ��֮��			
			ios.post([&, result]() {result->insertPosition = _STD move(sessionStorage.insert(sessionStorage.end(), result)); });
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
			corePtr->GetTrace()->DebugPoint(LogMsg::CloseAllSocket);
			if (isDestruct)
			{
				AsyncCloseAll();
				return;
			}
			ios.post(_BOOST bind(&TSelf::AsyncCloseAll, this));
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
		TWorker* worker_;
		//_ASIO strand strand;
		// ��ʾ�Ƿ�������״̬
		bool isDestruct;

		TCore* corePtr;
		_ASIO io_service& ios;
		_BOOST atomic<unsigned int> count;

		TResolver resolver;
		_BOOST system::error_code ignore;

		_STD allocator<TSession> alloc;
		// FIX:��(��freelock���ҵ��ģ��Ƿ���ô�����)
		TPool sessionPool;
		// ������������ָ��洢

		TStorage sessionStorage;

		void InsertSession(TSession* session)
		{
			session->insertPosition = _STD move(sessionStorage.insert(sessionStorage.end(), session));
		}
		void AsyncCloseAll()
		{
			for each (auto& var in sessionStorage)
			{
				var->close(ignore);
			}
		}

		void FreeSession(TSession* ptr)
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::FreeSession);

			// ����ʱ��erase��������ʱ��
			if (!isDestruct)
			{
				sessionStorage.erase(ptr->insertPosition);
			}

			// FIX: cpu
			// �����������ʹ��lockfree���ܻ������ٶ�
			sessionPool.destruct<true>(ptr);
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