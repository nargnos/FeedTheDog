#pragma once
namespace FeedTheDog
{
	class SpinLock :
		_BOOST noncopyable
	{
	public:
		SpinLock();
		inline bool TryLock();
		inline void Lock();
		inline void Unlock();
	private:
		inline void ThreadYield(unsigned int k);
		_STD atomic_flag lock = ATOMIC_VAR_INIT(false);
	};
	class SpinLockGuard :
		public _BOOST noncopyable
	{
	public:
		SpinLockGuard(SpinLock& lock);

		~SpinLockGuard();

	private:
		SpinLock& lock_;
	};

}  // namespace FeedTheDog
