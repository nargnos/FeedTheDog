#include "stdafx.h"
#include "SpinLock.h"

namespace FeedTheDog
{
	SpinLock::SpinLock()
	{
	}
	bool SpinLock::TryLock()
	{
		return !lock.test_and_set(_STD memory_order_acquire);
	}
	void SpinLock::Lock()
	{
		unsigned int i = 0;
		while (!TryLock())
		{
			ThreadYield(++i);
		}
	}
	void SpinLock::Unlock()
	{
		lock.clear(_STD memory_order_release);
	}
	void SpinLock::ThreadYield(unsigned int k)
	{
		if (k < 4)
		{
		}
#if defined( BOOST_SMT_PAUSE )
		else if (k < 16)
		{
			BOOST_SMT_PAUSE
		}
#endif
		else
		{
			_STD this_thread::yield();
		}
	}
	SpinLockGuard::SpinLockGuard(SpinLock & lock) :
		lock_(lock)
	{
		lock_.Lock();
	}
	SpinLockGuard::~SpinLockGuard()
	{
		lock_.Unlock();
	}
}  // namespace FeedTheDog
