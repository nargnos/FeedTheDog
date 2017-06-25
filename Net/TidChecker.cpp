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
		// ������Խ�ֹ���߳�ʹ�ö���
		assert(tid_ == pthread_self());
	}

	TidChecker::~TidChecker()
	{
		CheckTid();
	}

#endif 
}  // namespace Detail