#include "Util.h"
#include "Logger.h"
void SetAffinity(pthread_t t, unsigned int core)
{
	cpu_set_t set;

	CPU_ZERO(&set);
	CPU_SET(core, &set);

	if (pthread_setaffinity_np(t, sizeof(set), &set) == -1)
	{
		TRACEERRNO(LogPriority::Warning);
	}
}
