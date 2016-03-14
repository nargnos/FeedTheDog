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
		typedef typename TProtocol::socket TSocket;
		typedef typename _BOOST shared_mutex TMutex;
		typedef _BOOST shared_lock<TMutex> ReadLock;
		typedef _BOOST unique_lock<TMutex> WriteLock;

		// session������ί��
		typedef _BOOST _bi::bind_t<
			void, _BOOST _mfi::mf1<void, TSelf, TSession *>,
			_BOOST _bi::list2<_BOOST _bi::value<TSelf*>, _BOOST arg<1>>
		> TFreeSessionDelegate;

		SessionPool(TCore* core, _ASIO io_service& io) :
			corePtr(core),
			ios(io),
			count(0),
			sessionPool(alloc, 0),
			freeSessionDelegate(_BOOST bind(&TSelf::Free, this, _1))
		{
			isDestruct = false;
		}

		~SessionPool()
		{
			isDestruct = true;
			if (count > 0)
			{
				CloseAll();
			}
		}

		// ���ڶ���̷߳���������accept�ص�ʱ�ڵ�ǰ�̴߳���������һworker��session��
		virtual shared_ptr<TSession> NewSession(const char* serviceName) override
		{
			++count;
			TSession* result = NULL;

			// ����session
			result = sessionPool.construct<true, false>(corePtr, _STD ref(ios));

			// �ɶ��߳�ͬʱinsert�����ǲ�����eraseͬʱ���룬������sharedlock������Ϊreadlock
			ReadLock lock(mutex);
			// �������λ�ã�ɾ��ʱʹ��
			// FIX: ��������cpu
			result->mapPosition = sessionMap.insert(TMapValue(serviceName, result));
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
#ifdef _DEBUG
			corePtr->GetTrace()->TracePoint(LogMsg::CloseAllSocket);
#endif // _DEBUG
			// lock
			ReadLock lock(mutex);
			for each (auto& var in sessionMap)
			{
				CloseSession(var.second);
			}
		}
		// �Ƴ�����sessionǰ�Ȼ�ֹͣ���񣬴�ʱ����Ӧ�ܾ�������
		// ֻ������״̬����
		virtual void RemoveServiceSession(const char* serviceName) override
		{
			// lock
			ReadLock readlock(mutex);
			auto& it = sessionMap.find(serviceName);
			while (it != sessionMap.end())
			{
				CloseSession(it->second);
				it->second->isErased = true;
			}
			readlock.unlock();
			// lock
			WriteLock lock(mutex);
			sessionMap.unsafe_erase(serviceName);
		}
	private:
		// ��ʾ�Ƿ�������״̬
		bool isDestruct;
		// �򻯵���bind
		TFreeSessionDelegate freeSessionDelegate;

		TCore* corePtr;
		_ASIO io_service& ios;
		_BOOST atomic<unsigned int> count;

		// lock		
		_BOOST shared_mutex mutex;

		// ��(��freelock���ҵ��ģ��Ƿ���ô�����)
		_STD allocator<TSession> alloc;

		TPool sessionPool;
		// ������������ָ��洢
		// FIX: ����insert��erase�ķ�cpu�ǳ���
		TMap sessionMap;

		void CloseSession(TSession* ptr)
		{
			if (!ptr->isClosed)
			{
				ptr->isClosed = true;
				auto& socket = ptr->GetSocket();
				socket.shutdown(_ASIO socket_base::shutdown_type::shutdown_both);
				socket.close();
			}
		}
		void Free(TSession* ptr)
		{
			// lock
			WriteLock lock(mutex);
			if (!ptr->isErased)
			{
				// ����ʱ��erase��������ʱ��
				if (!isDestruct)
				{
					// FIX: cpu
					sessionMap.unsafe_erase(ptr->mapPosition);
				}
				ptr->isErased = true;
			}
			lock.unlock();
			// FIX: cpu
			sessionPool.destruct<true>(ptr);
			--count;
		}
		void DestructSession(TSession* ptr)
		{
			sessionPool.destruct<true>(ptr);
		}
	};

}  // namespace FeedTheDog