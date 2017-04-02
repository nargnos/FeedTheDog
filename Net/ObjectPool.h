
#ifndef OBJECTPOOL_H_
#define OBJECTPOOL_H_
#include <memory>
#include <stack>
#include <algorithm>
#include <cassert>
#include "Noncopyable.h"

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


// 非线程安全，生成的智能指针对象析构时也非线程安全
template<typename TObj, typename TObjAllocator = ObjAllocator<TObj>>
class ObjectPool :
	public std::enable_shared_from_this<ObjectPool<TObj, TObjAllocator>>,
	public Noncopyable
{
public:
	using TSelf = ObjectPool<TObj, TObjAllocator>;
	static_assert(!std::is_void<TObj>::value && sizeof(TObj) > 0, "TObj 类型错误");
	using TPollWeakPtr = std::weak_ptr<ObjectPool>;
	using TObjPtr = std::shared_ptr<TObj>;
	struct UniPtrDel :public std::default_delete<TObj>
	{
		UniPtrDel(const TPollWeakPtr& weak) :weak_(weak)
		{

		}
		void operator()(TObj* obj)
		{
			ObjectPool::Recycle(weak_, obj);
		}
		TPollWeakPtr weak_;
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
		return TObjPtr(result, [pool = self_](TObj* obj) mutable {
			Recycle(pool, obj);
		});
	}
	template<typename... TArgs>
	TObjUniPtr NewUnique(TArgs&&... args)
	{
		auto result = GetObj(std::forward<TArgs>(args)...);
		return TObjUniPtr(result, UniPtrDel(self_));
	}

	// maxCount 等于0表示不限制保存对象个数（存储的个数不是总共能建立的个数）
	static std::shared_ptr<ObjectPool> Create(size_t maxCount = 0, TObjAllocator&& alloc = TObjAllocator())
	{
		auto result = std::shared_ptr<ObjectPool>(new ObjectPool(maxCount, std::move(alloc)));
		result->InitTask();
		return result;
	}
	// 回收所有内存
	void Clear()
	{
		DeleteAll();
	}

private:
	void DeleteAll()
	{
		while (objs_.empty())
		{
			alloc_.Delete(objs_.top());
			objs_.pop();
			DecCount();
		}
	}
	ObjectPool(size_t maxCount, TObjAllocator&&  alloc) :
		alloc_(std::move(alloc)),
		maxCount_(maxCount)
	{
	}
	void InitTask()
	{
		self_ = this->shared_from_this();
	}

	static void Recycle(std::weak_ptr<ObjectPool>& pool, TObj* obj)
	{
		if (!pool.expired())
		{
			auto poolObj = pool.lock();
			if (poolObj->PushObj(obj))
			{
				poolObj->alloc_.Clear(obj);
				return;
			}
		}
		delete obj;
	}
	bool PushObj(TObj* obj)
	{
		// assert(!IsExist(obj));
		if (maxCount_ == 0 || Count() < maxCount_)
		{
			objs_.push(obj);
			IncCount();
			return true;
		}
		return false;
	}
	inline TObj * PopObj()
	{
		auto result = objs_.top();
		objs_.pop();
		DecCount();
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
		if (objs_.empty())
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

	inline void IncCount()
	{
		++count_;
	}
	inline void DecCount()
	{
		--count_;
	}
	inline size_t Count() const
	{
		return count_;
	}
	std::stack<TObj*> objs_;
	TObjAllocator alloc_;
	TPollWeakPtr self_;
	size_t maxCount_;
	size_t count_;
};
#endif // OBJECTPOOL_H_



