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
		public ISessionPool<TProtocol>
	{
	public:
		typedef typename SessionPool<TProtocol> TSelf;
		typedef typename SessionPoolTrait::TSession<TProtocol>::type TSession;
		typedef typename SessionPoolTrait::TPool<TProtocol>::type TPool;
		typedef typename SessionPoolTrait::TSessionMultiMap<TProtocol>::type TMap;
		typedef typename TMap::iterator TMapIterator;
		typedef typename TMap::value_type TMapValue;
		typedef typename CoreTrait::TCore TCore;
		typedef typename CoreTrait::TWorker TWorker;

		typedef typename TProtocol::socket TSocket;
		typedef typename _BOOST shared_mutex TMutex;
		typedef _BOOST shared_lock<TMutex> ReadLock;
		typedef _BOOST unique_lock<TMutex> WriteLock;
		typedef typename TProtocol::resolver TResolver;

		// session������ί��
		typedef _BOOST _bi::bind_t<
			void, _BOOST _mfi::mf1<void, TSelf, TSession *>,
			_BOOST _bi::list2<_BOOST _bi::value<TSelf*>, _BOOST arg<1>>
		> TFreeSessionDelegate;

		SessionPool(TCore* core, TWorker* worker, _ASIO io_service& io) :
			corePtr(core),
			ios(io),
			count(0),
			sessionPool(alloc, 0),
			freeSessionDelegate(_BOOST bind(&TSelf::Free, this, _1)),
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
		virtual shared_ptr<TSession> NewSession(const char* serviceName) override
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::NewSession, false, 0, serviceName);
			++count;
			// FIX: ����ʲô����ķ������Ի�
			// ����session
			TSession* result = sessionPool.construct<true, false>(worker_);

			// ������eraseִ��ʱ����Ի���insertִ��֮������������ʱ�������δ����map����λ�õ����
			ios.post(_BOOST bind(&TSelf::InsertSession, this, serviceName, result));

			return shared_ptr<TSession>(result, freeSessionDelegate);
		}


		virtual unsigned int GetSessionCount() const override
		{
			return count;
		}
		// ��������ʱ��ֹͣǰ����
		virtual void CloseAll() override
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

		// �Ƴ�����sessionǰ�Ȼ�ֹͣ���񣬴�ʱ����Ӧ�ܾ�������
		// ֻ������״̬����
		virtual void RemoveServiceSession(const char* serviceName) override
		{
			ios.post(_BOOST bind(&TSelf::RemoveServiceSessionStrand, this, serviceName));
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
		// �򻯵���bind
		TFreeSessionDelegate freeSessionDelegate;

		TCore* corePtr;
		_ASIO io_service& ios;
		_BOOST atomic<unsigned int> count;

		TResolver resolver;
		_BOOST system::error_code ignore;

		_STD allocator<TSession> alloc;
		// FIX:��(��freelock���ҵ��ģ��Ƿ���ô�����)
		TPool sessionPool;
		// ������������ָ��洢
		// FIX: ����insert��erase�ķ�cpu�ǳ���
		TMap sessionMap;

		void InsertSession(const char* serviceName, TSession* session)
		{
			session->mapPosition = sessionMap.insert(TMapValue(serviceName, session));
		}
		void AsyncCloseAll()
		{
			for each (auto& var in sessionMap)
			{
				var.second->close(ignore);
			}
		}
		void FreeSession(TSession* ptr)
		{
			corePtr->GetTrace()->DebugPoint(LogMsg::FreeSession);

				// ����ʱ��erase��������ʱ��
			if (!isDestruct)
			{
				// FIX: cpu
				sessionMap.erase(ptr->mapPosition);
			}

			// FIX: cpu
			// �����������ʹ��lockfree���ܻ������ٶ�
			sessionPool.destruct<true>(ptr);
			--count;
		}
		void RemoveServiceSessionStrand(const char* serviceName)
		{
			auto& it = sessionMap.find(serviceName);
			auto eraseIt = it;
			while (it != sessionMap.end())
			{
				it->second->close(ignore);
			}
			sessionMap.erase(eraseIt, sessionMap.end());
		}
		void Free(TSession* ptr)
		{
			if (isDestruct)
			{
				// ioservice����free֮ǰֹͣ�����Բ���post
				FreeSession(ptr);
				return;
			}
			ios.post(_BOOST bind(&TSelf::FreeSession, this, ptr));
		}
	};

}  // namespace FeedTheDog