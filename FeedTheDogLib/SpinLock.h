#pragma once
namespace FeedTheDog
{
	class SpinLock final:
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
	class SpinLockGuard final :
		public _BOOST noncopyable
	{
	public:
		explicit SpinLockGuard(SpinLock& lock);

		~SpinLockGuard();

	private:
		SpinLock& lock_;
	};

}  // namespace FeedTheDog
