#ifndef UTIL_H_
#define UTIL_H_
#include <pthread.h>
void SetAffinity(pthread_t t, unsigned int core);
// 弄短一些
#define LIKELY(cond) __glibc_likely(cond)
#define UNLIKELY(cond) __glibc_unlikely(cond)
#define EXPECT(val,exp) __builtin_expect((val),(exp))
#endif // UTIL_H_

