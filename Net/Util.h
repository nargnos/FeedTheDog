#ifndef UTIL_H_
#define UTIL_H_
#include <pthread.h>
void SetAffinity(pthread_t t, unsigned int core);
#endif // UTIL_H_

