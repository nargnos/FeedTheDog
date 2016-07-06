#pragma once
#include "SpinLock.h"
#include "DlmallocAllocatorNewDelete.h"
#include "PoolTrait.h"
namespace FeedTheDog
{
	enum SmartPtrTypeID :
		unsigned char
	{
		Pointer,
		Shared,
		Unique
	};

	// FIX: 代码膨胀
	// 简化一些创建对象步骤
	// 也能创建一些数组，未做广泛测试
	// 只弄boost的两个库
	// 需要注意pool快一点，但是不会在析构时析构掉对象
	template<typename TObject,
		template<typename, typename...> class TPoolType = _BOOST object_pool,
		typename TLock = SpinLock,
		typename TAllocator = DlmallocAllocatorNewDelete,
		// 在内部定义此类型会极大的影响VS自动提示速度，所以只能放到这里
		typename TPool = typename _STD conditional<
		PoolTrait<TPoolType>::Value == MemoryPoolType::BoostObjectPool,
		_BOOST object_pool<TObject, TAllocator>,
		typename _STD conditional<
		PoolTrait<TPoolType>::Value == MemoryPoolType::BoostPool,
		_BOOST pool<TAllocator>,
		void
		>::type
	>::type>
		class ObjectPool :
		public _BOOST noncopyable
	{
	public:
		typedef TObject TObject;
		class ObjectPointer;
		class ObjectDeletor;

		// 返回指针类型
		// Shared -> shared_ptr<TObject>
		// Unique -> unique_ptr<TObject, ObjectDeletor>
		// Default -> ObjectPointer 手动控制删除
		template<SmartPtrTypeID type>
		using TObjectPtr = typename _STD conditional<
			type == SmartPtrTypeID::Shared,
			shared_ptr<TObject>,
			typename _STD conditional<
			type == SmartPtrTypeID::Unique,
			unique_ptr<TObject, ObjectDeletor>,
			ObjectPointer
			>::type
		>::type;

		explicit ObjectPool(size_t poolNextSize = 32) :
			isDestroying_(false),
			pool_(CreatePool<PoolTrait<TPoolType>::Value>(poolNextSize))
		{

			del_.pool_ = this;
		}

		~ObjectPool()
		{
			isDestroying_ = true;
			pool_.reset();
		}

		// 创建对象，需要自己保证pool在对象后析构
		template<SmartPtrTypeID typeID, typename... TArgs>
		TObjectPtr<typeID> FASTCALL Construct(TArgs&&... args)
		{
			auto result = Malloc();
			NewObject<TObject, TArgs...>(result, _STD forward<TArgs>(args)...);
			return WrapPtr<typeID>(result);
		}

	private:

		void Destroy(TObject* ptr)
		{
			assert(pool_->is_from(ptr));
			DestroyObject<TObject>(ptr);
			Free(ptr);
		}
		inline const ObjectDeletor& GetDeletor() const
		{
			return del_;
		}
		template<MemoryPoolType poolTypeID>
		unique_ptr<TPool> CreatePool(size_t poolNextSize);

		template<>
		unique_ptr<TPool> CreatePool<MemoryPoolType::BoostObjectPool>(size_t poolNextSize)
		{
			return make_unique<TPool>(poolNextSize);
		}

		template<>
		unique_ptr<TPool> CreatePool<MemoryPoolType::BoostPool>(size_t poolNextSize)
		{
			return make_unique<TPool>(sizeof(TObject), poolNextSize);
		}

		template<SmartPtrTypeID typeID>
		inline TObjectPtr<typeID> WrapPtr(TObject* ptr)
		{
			return TObjectPtr<typeID>(ptr, GetDeletor());
		}

		template<>
		inline ObjectPointer WrapPtr<SmartPtrTypeID::Pointer>(TObject* ptr)
		{
			ObjectPointer result;
			result.first = this;
			result.second = ptr;
			return result;
		}

		template<typename T, typename... TArgs>
		inline void NewObject(_STD enable_if_t<!_STD is_pod<TObject>::value, T*> ptr, TArgs&&... args)
		{
			try
			{
				new (ptr) T(_STD forward<TArgs>(args)...);
			}
			catch (...)
			{
				Free(ptr);
				throw;
			}
		}
		template<typename T, typename... TArgs>
		inline static void NewObject(_STD enable_if_t<_STD is_pod<TObject>::value, T*> ptr, TArgs&&... args)
		{

		}

		template<typename T>
		inline static void DestroyObject(_STD enable_if_t<!_STD is_pod<TObject>::value, T*> ptr)
		{
			ptr->~T();
		}
		template<typename T>
		inline static void DestroyObject(_STD enable_if_t<_STD is_pod<TObject>::value, T*> ptr)
		{

		}

		// new对象和malloc分开，防止锁时间太长

		TObject* Malloc()
		{
			assert(pool_);
			TObject* result = nullptr;
			{
				_STD lock_guard<TLock> guard(lock_);
				do
				{
					result = reinterpret_cast<TObject*>(pool_->malloc());
				} while (result == nullptr);
			}
			return result;
		}


		inline void Free(TObject* ptr)
		{
			assert(pool_);
			_STD lock_guard<TLock> guard(lock_);
			pool_->free(ptr);
		}


		ALIGN TLock lock_;
		ALIGN ObjectDeletor del_;
		unique_ptr<TPool> pool_;
		bool isDestroying_;
	};

#define OBJECT_POOL_TEMPLATE template<typename TObject, template<typename,typename ...> class TPoolType, typename TLock, typename TAllocator,typename TPool>
#define OBJECT_POOL_DEFINE ObjectPool<TObject, TPoolType, TLock, TAllocator,TPool>

	OBJECT_POOL_TEMPLATE
		class OBJECT_POOL_DEFINE::ObjectPointer :
		protected _STD pair<
		OBJECT_POOL_DEFINE*,
		TObject*>
	{
	public:
		friend class OBJECT_POOL_DEFINE;
		ObjectPointer() = default;
		~ObjectPointer() = default;
		inline void Delete()
		{
			ObjectDeletor::Destroy(first, second);
			second = nullptr;
		}
		void Clear()
		{
			Assign(nullptr);
		}
		operator bool() const
		{
			return second != nullptr;
		}
		bool operator==(const ObjectPointer& val)const
		{
			return (first == val.first) && (val.second == second);
		}
		bool operator!=(const ObjectPointer& val)const
		{
			return !operator==(val);
		}
		bool operator==(nullptr_t)const
		{
			return !operator bool();
		}
		bool operator!=(nullptr_t)const
		{
			return operator bool();
		}
		friend bool operator==(nullptr_t, const ObjectPointer& val)
		{
			return !val.operator bool();
		}
		friend bool operator!=(nullptr_t, const ObjectPointer& val)
		{
			return val.operator bool();
		}
		TObject* operator->()
		{
			return Get();
		}
		TObject* Get()
		{
			return second;
		}
	private:
		void Assign(TObject* ptr)
		{
			second = ptr;
		}
	};
	OBJECT_POOL_TEMPLATE
		class OBJECT_POOL_DEFINE::ObjectDeletor
	{
	public:
		ObjectDeletor() = default;
		~ObjectDeletor() = default;
		void operator()(TObject* ptr)
		{
			Destroy(pool_, ptr);
		}
		static void Destroy(ObjectPool* pool, TObject* ptr)
		{
			assert(pool != nullptr);
			if (ptr == nullptr || pool->isDestroying_)
			{
				return;
			}
			pool->Destroy(ptr);
		}

		ObjectPool* pool_;
	};

}  // namespace FeedTheDog
