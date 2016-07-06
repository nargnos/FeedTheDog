#pragma once
#include "ServiceManager.h"
namespace FeedTheDog
{
	class WorkerFriendProxy;

	struct ServiceDefines :WorkerDefines
	{
		using TServiceManager = ServiceManager;

		using TWorker = typename TServiceManager::TWorker;
		using TWorkerPool = typename TServiceManager::TWorkerPool;
		using TWorkerRef = const TWorkerPool::WorkerPtr&;

		/*	template<typename TProtocol>
			using TSocketPool = typename TWorker::template TSocketPool<TProtocol>;
	*/
		template<typename TProtocol>
		using TSocketPool = typename TWorker::template TSocketPool<TProtocol>;

		template<typename TProtocol>
		using TSessionPool = typename TWorker::template TSessionPool<TProtocol>;

		template<typename TProtocol>
		using TResolver = typename TWorker::template TResolver<TProtocol>;

		template<typename TProtocol, SmartPtrTypeID ptrType>
		using TSocketPoolResult = typename TSocketPool<TProtocol>::template TObjectPtr<ptrType>;

		template<typename TFunc, typename... TArgs>
		using TFutureType = TWorkerPool::TFuncResult<TFunc, TArgs... >;

		template<BufferType bufferType, SmartPtrTypeID ptrType>
		using TBufferPoolResult = typename TWorker::template TBufferPoolType<bufferType>::template TObjectPtr<ptrType>;

		template<typename TProtocol>
		using TSocketPointer = typename TSocketPool<TProtocol>::ObjectPointer;

		template<BufferType bufferType>
		using TBufferPointer = typename TWorker::template TBufferPoolType<bufferType>::ObjectPointer;

		template<typename TProtocol>
		using TSessionPointer = typename TSessionPool<TProtocol>::ObjectPointer;
	};
	// FIX: 不够易用
	class ServiceBaseImpl :
		protected ServiceDefines,
		public _BOOST noncopyable
	{
	public:
		explicit ServiceBaseImpl(TWorkerPool& workerPool);

		virtual ~ServiceBaseImpl();


		// 以下函数是对workerpool和worker的桥接


		inline _ASIO io_service& GetIoService()
		{
			return workerPool_.GetIoService();
		}



		template<typename TFunc>
		inline void PostTask(TFunc&& handler)
		{
			workerPool_.PostTask(_STD forward<TFunc>(handler));
		}

		template<typename TFunc, typename... TArgs>
		inline _STD future<_STD result_of_t<_STD decay_t<TFunc>(_STD decay_t<TArgs>...)>>
			Async(TFunc&& func, TArgs&&... args)
		{
			return workerPool_.Async(_STD forward<TFunc>(func), _STD forward<TArgs>(args)...);
		}

		template<WorkerSelector selector>
		inline TWorkerRef GetWorker() const
		{
			return workerPool_.GetWorker<selector>();
		}
	protected:


	private:



		TWorkerPool& workerPool_;
	};


}  // namespace FeedTheDog
