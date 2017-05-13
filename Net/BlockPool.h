﻿#ifndef BLOCKPOOL_H_
#define BLOCKPOOL_H_
#include <vector>
#include <type_traits>
#include "ObjectPool.h"
#include "Block.h"
#include "Noncopyable.h"
namespace Detail
{
	template<typename T>
	class BlockAllocator
	{
	public:
	public:
		constexpr BlockAllocator() = default;

		void Reset(T* obj, size_t size)
		{
			obj->resize(size);
		}
		void Reset(T* obj)
		{
			obj->ResizeToMax();
		}
		inline static void Clear(T* obj)
		{
		}

		T* New(size_t size)
		{
			// 可在这里分配对齐
			return new T(size);
		}
		T* New()
		{
			return new T(T::Max);
		}
		inline static void Delete(T* obj)
		{
			delete obj;
		}

	};


	template<>
	struct ObjAllocator<SmallBlock> :public BlockAllocator<SmallBlock>
	{
		using BlockAllocator<SmallBlock>::BlockAllocator;
	};
	template<>
	struct ObjAllocator<BigBlock> :public BlockAllocator<BigBlock>
	{
		using BlockAllocator<BigBlock>::BlockAllocator;
	};


	// 感觉其实没什么用	
	class BlockPool :
		public Noncopyable
	{
	public:
		using Ptr = std::shared_ptr<Block>;
		BlockPool();
		~BlockPool() = default;
		// 填0分配默认大小
		Ptr New(size_t size);
		Ptr New();
	private:
		std::shared_ptr<ObjectPool<BigBlock>> bBuff_;
		std::shared_ptr<ObjectPool<SmallBlock>> sBuff_;
	};

}  // namespace Detail
using BlockPtr = Detail::BlockPool::Ptr;
#endif // BLOCKPOOL_H_

