#include "stdafx.h"
#include "SpinLock.h"

namespace FeedTheDog
{
	bool SpinLock::try_lock()
	{
		return !lock_.test_and_set(_STD memory_order_acquire);
	}
	void SpinLock::lock()
	{
		unsigned int i = 0;
		while (!try_lock())
		{
			ThreadYield(++i);
		}
	}
	void SpinLock::unlock()
	{
		lock_.clear(_STD memory_order_release);
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
}  // namespace FeedTheDog
