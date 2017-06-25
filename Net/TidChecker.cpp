#include "TidChecker.h"
namespace Detail
{
#ifdef NDEBUG
	void TidChecker::CheckTid()
	{
	}
#else
	TidChecker::TidChecker() :
		tid_(pthread_self())
	{
	}

	void TidChecker::CheckTid() const
	{
		// 这里断言禁止跨线程使用对象
		assert(tid_ == pthread_self());
	}

	TidChecker::~TidChecker()
	{
		CheckTid();
	}

#endif 
}  // namespace Detail