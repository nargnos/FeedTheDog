#pragma once
namespace FeedTheDog
{
	// 函数名按照stl的以兼容lock_guard
	class SpinLock :
		_BOOST noncopyable
	{
	public:
		bool try_lock();
		void lock();
		void unlock();
	private:
		inline static void ThreadYield(unsigned int k);
		_STD atomic_flag lock_ = ATOMIC_VAR_INIT(false);
		
	};


}  // namespace FeedTheDog
