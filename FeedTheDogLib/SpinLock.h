#pragma once
namespace FeedTheDog
{
	class SpinLock :
		_BOOST noncopyable
	{
	public:
		SpinLock()
		{
		}
		inline bool TryLock()
		{
			return !lock.test_and_set(_STD memory_order_acquire);
		}
		inline void Lock()
		{
			unsigned int i = 0;
			while (!TryLock())
			{
				ThreadYield(++i);
			}
		}
		inline void Unlock()
		{
			lock.clear(_STD memory_order_release);
		}
	private:
		inline void ThreadYield(unsigned int k)
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
		_STD atomic_flag lock = ATOMIC_VAR_INIT(false);
	};
	class SpinLockGuard :
		public _BOOST noncopyable
	{
	public:
		SpinLockGuard(SpinLock& lock) :
			lock_(lock)
		{
			lock_.Lock();
		}

		~SpinLockGuard()
		{
			lock_.Unlock();
		}

	private:
		SpinLock& lock_;
	};

}  // namespace FeedTheDog
