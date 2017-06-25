
#ifndef MemoryPool_H_
#define MemoryPool_H_
#include <memory>
#include <stack>
#include <algorithm>
#include <cassert>
#include "Noncopyable.h"
#include "Util.h"
namespace Detail
{

	template<typename TObj>
	class MemAllocator
	{
	public:
		// 可非静态
		template<typename... TArgs>
		inline static void Reset(TObj* obj, TArgs&&... args)
		{

		}
		// 可非静态
		template<typename... TArgs>
		inline static TObj* New(TArgs&&... args)
		{
			return new TObj(std::forward<TArgs>(args)...);
		}
		// 可非静态
		inline static void Clear(TObj* obj)
		{}
		// 静态，只能是单纯delete
		inline static void Delete(TObj* obj)
		{
			delete obj;
		}
	};


	// TODO: 需要验证线程安全
	template<typename TObj, int Max = 0, typename TObjAllocator = MemAllocator<TObj>>
	class MemoryPool :
		public std::enable_shared_from_this<MemoryPool<TObj, Max, TObjAllocator>>,
		public Noncopyable
	{
	public:
		using TSelf = MemoryPool<TObj, Max, TObjAllocator>;
		static_assert(!std::is_void<TObj>::value && sizeof(TObj) > 0, "TObj 类型错误");
		using TPollWeakPtr = std::weak_ptr<MemoryPool>;
		using TObjPtr = std::shared_ptr<TObj>;
		using TUniPtr = std::unique_ptr<TObj>;
		~MemoryPool() = default;
		template<typename... TArgs>
		TObjPtr New(TArgs&&... args)
		{
			auto result = GetObj(std::forward<TArgs>(args)...);
			return TObjPtr(result, [](TObj* obj) mutable {
				Recycle(obj);
			});
		}
		static const std::shared_ptr<MemoryPool>& Instance()
		{
			static thread_local std::shared_ptr<MemoryPool> instance = Create();
			return instance;
		}

		// 回收所有内存
		void Clear()
		{
			DeleteAll();
		}

	private:

		void DeleteAll()
		{
			if (objs_.empty())
			{
				return;
			}
			std::stack<TUniPtr>().swap(objs_);
		}
		explicit MemoryPool(TObjAllocator&&  alloc) :
			alloc_(std::move(alloc))
		{
		}

		bool PushObj(TObj* obj)
		{
			if (Max == 0 || objs_.size() <= Max)
			{
				objs_.push(TUniPtr(obj));
				return true;
			}
			return false;
		}
		TObj * PopObj()
		{
			auto result = objs_.top().release();
			objs_.pop();
			return result;
		}
		template<typename ...TArgs>
		TObj* GetObj(TArgs&&... args)
		{
			TObj* result = nullptr;
			if (UNLIKELY(objs_.empty()))
			{
				result = alloc_.New(std::forward<TArgs>(args)...);
			}
			else
			{
				result = PopObj();
				alloc_.Reset(result, std::forward<TArgs>(args)...);
			}
			return result;
		}
		static std::shared_ptr<MemoryPool> Create(TObjAllocator&& alloc = TObjAllocator())
		{
			auto result = std::shared_ptr<MemoryPool>(new MemoryPool(std::move(alloc)));
			assert(WeakPtr().expired());
			WeakPtr() = result;
			return result;
		}
		static void Recycle(TObj* obj)
		{
			auto pool = CWeak().lock();

			if (__glibc_likely((bool)pool))
			{
				if (pool->PushObj(obj))
				{
					pool->alloc_.Clear(obj);
					return;
				}
			}
			TObjAllocator::Delete(obj);
		}
		static std::weak_ptr<MemoryPool>& WeakPtr()
		{
			static thread_local std::weak_ptr<MemoryPool> instanceWeak;
			return instanceWeak;
		}


		static std::weak_ptr<MemoryPool> CWeak()
		{
			return WeakPtr();
		}
		std::stack<TUniPtr> objs_;
		TObjAllocator alloc_;

	};


}  // namespace Detail
#endif // MemoryPool_H_
