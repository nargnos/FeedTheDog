#pragma once
#include "..\Common.h"
#include "IPool.h"
namespace FeedTheDog
{
	__interface IMemoryPool :
		public IPool
	{
		int GetBufferSize() const = 0;
		shared_ptr<unsigned char> Malloc() = 0;
	};
}  // namespace FeedTheDog
