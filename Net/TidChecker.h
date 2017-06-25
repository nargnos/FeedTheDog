#ifndef TIDCHECKER_H_
#define TIDCHECKER_H_
#include <pthread.h>
#include <cassert>
namespace Detail
{
	class TidChecker
	{
	public:

#ifdef NDEBUG
		constexpr TidChecker() = default;
		~TidChecker() = default;
	protected:
		static void CheckTid();
#else
		TidChecker();
		~TidChecker();
	protected:
		void CheckTid()const;
	private:
		pthread_t tid_;
#endif // NDEBUG
	};
}  // namespace Detail
#endif // TIDCHECKER_H_

