#include "BlockPool.h"

BlockPool::BlockPool() :
	bBuff_(ObjectPool<BigBlock>::Instance()),
	sBuff_(ObjectPool<SmallBlock>::Instance())
{

}

BlockPool::UniPtr BlockPool::NewUnique(size_t size)
{
	UniPtr result;
	if (__glibc_likely(size == 0))
	{
		result = bBuff_->NewUnique();
	}
	else if (__glibc_likely(size > SmallBlock::Max))
	{
		result = bBuff_->NewUnique(size);
	}
	else
	{
		result = sBuff_->NewUnique(size);
	}
	return result;
}

BlockPool::Ptr BlockPool::New(size_t size)
{
	Ptr result;
	if (__glibc_likely(size == 0))
	{
		result = bBuff_->New();
	}
	else if (__glibc_likely(size > SmallBlock::Max))
	{
		result = bBuff_->New(size);
	}
	else
	{
		result = sBuff_->New(size);
	}
	return result;
}

BlockPool::UniPtr BlockPool::NewUnique()
{
	return bBuff_->NewUnique();
}

BlockPool::Ptr BlockPool::New()
{
	return bBuff_->New();
}
