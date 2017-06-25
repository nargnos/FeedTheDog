﻿#include "BlockPool.h"
namespace Detail
{

	BlockPool::BlockPool() :
		bBuff_(BigBlockPool::Instance()),
		sBuff_(SmallBlockPool::Instance())
	{

	}

	BlockPool::Ptr BlockPool::New(size_t size)
	{
		Ptr result;
		if (__glibc_likely(size == 0))
		{
			result = bBuff_->New();
		}
		else if (__glibc_likely(size <= SmallBlock::Max))
		{
			result = sBuff_->New(size);
		}
		else if (size <= BigBlock::Max)
		{
			result = bBuff_->New(size);
		}
		else
		{
			result = bBuff_->New();
		}
		return result;
	}

	BlockPool::Ptr BlockPool::New()
	{
		return bBuff_->New();
	}

}  // namespace Detail