
#ifndef OBJECTPOOL_H_
#define OBJECTPOOL_H_
#include <memory>
#include <stack>
#include <algorithm>
#include <cassert>
#include "Noncopyable.h"
namespace Detail
{

	template<typename TObj>
	class ObjAllocator
	{
	public:
		template<typename... TArgs>
		inline static void Reset(TObj* obj, TArgs&&... args)
		{

		}
		template<typename... TArgs>
		inline static TObj* New(TArgs&&... args)
		{
			return new TObj(std::forward<TArgs>(args)...);
		}
		inline static void Clear(TObj* obj)
		{}
		inline static void Delete(TObj* obj)
		{
			delete obj;
		}
	};


	// TODO: 需要验证线程安全
	template<typename TObj, int Max = 0, typename TObjAllocator = ObjAllocator<TObj>>
	class ObjectPool :
		public std::enable_shared_from_this<ObjectPool<TObj, Max, TObjAllocator>>,
		public Noncopyable
	{
	public:
		using TSelf = ObjectPool<TObj, Max, TObjAllocator>;
		static_assert(!std::is_void<TObj>::value && sizeof(TObj) > 0, "TObj 类型错误");
		using TPollWeakPtr = std::weak_ptr<ObjectPool>;
		using TObjPtr = std::shared_ptr<TObj>;
		struct UniPtrDel :public std::default_delete<TObj>
		{
			UniPtrDel() = default;
			void operator()(TObj* obj)
			{
				ObjectPool::Recycle(Weak(), obj);
			}
		};

		using TObjUniPtr = std::unique_ptr<TObj, UniPtrDel>;


		~ObjectPool()
		{
			DeleteAll();
		}
		template<typename... TArgs>
		TObjPtr New(TArgs&&... args)
		{
			auto result = GetObj(std::forward<TArgs>(args)...);
			return TObjPtr(result, [](TObj* obj) mutable {
				Recycle(Weak(), obj);
			});
		}
		template<typename... TArgs>
		TObjUniPtr NewUnique(TArgs&&... args)
		{
			auto result = GetObj(std::forward<TArgs>(args)...);
			return TObjUniPtr(result, UniPtrDel());
		}

		static const std::shared_ptr<ObjectPool>& Instance()
		{
			static thread_local std::shared_ptr<ObjectPool> instance = Create();
			return instance;
		}

		// 回收所有内存
		void Clear()
		{
			DeleteAll();
		}

	private:


		static const std::weak_ptr<ObjectPool>& Weak()
		{
			return instanceWeak;
		}
		void DeleteAll()
		{
			while (!objs_.empty())
			{
				alloc_.Delete(objs_.top());
				objs_.pop();
			}
		}
		explicit ObjectPool(TObjAllocator&&  alloc) :
			alloc_(std::move(alloc))
		{
		}


		static std::shared_ptr<ObjectPool> Create(TObjAllocator&& alloc = TObjAllocator())
		{
			auto result = std::shared_ptr<ObjectPool>(new ObjectPool(std::move(alloc)));
			assert(instanceWeak.expired());
			instanceWeak = result;
			return result;
		}
		static void Recycle(const std::weak_ptr<ObjectPool>& pool, TObj* obj)
		{
			if (__glibc_likely(!pool.expired()))
			{
				auto poolObj = pool.lock();
				poolObj->PushObj(obj);
				poolObj->alloc_.Clear(obj);
				return;
			}
			delete obj;
		}
		void PushObj(TObj* obj)
		{
			// assert(!IsExist(obj));
			if (Max == 0)
			{
				objs_.push(obj);
				return;
			}
			if (objs_.size() > Max)
			{
				return;
			}
			objs_.push(obj);
		}
		TObj * PopObj()
		{
			auto result = objs_.top();
			objs_.pop();
			return result;
		}
		bool IsExist(TObj* obj)
		{
			return std::find(objs_.begin(), objs_.end(), obj) != objs_.end();
		}

		template<typename ...TArgs>
		TObj* GetObj(TArgs&&... args)
		{
			TObj* result = nullptr;
			if (__glibc_unlikely(objs_.empty()))
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

		std::stack<TObj*> objs_;
		TObjAllocator alloc_;


		static thread_local std::weak_ptr<ObjectPool> instanceWeak;

	};


	template<typename TObj, int Max, typename TObjAllocator>
	thread_local std::weak_ptr<ObjectPool<TObj, Max, TObjAllocator>>
		ObjectPool<TObj, Max, TObjAllocator>::instanceWeak;
}  // namespace Detail
#endif // OBJECTPOOL_H_
