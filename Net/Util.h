#ifndef UTIL_H_
#define UTIL_H_
#include <type_traits>
#include <functional>
#include <pthread.h>
void SetAffinity(pthread_t t, unsigned int core);
#define CheckHandler(type,handler) static_assert(std::is_assignable<std::function<type>, decltype(handler)>::value,"function def")

// 弄短一些
#define LIKELY(cond) __glibc_likely(cond)
#define UNLIKELY(cond) __glibc_unlikely(cond)
#define EXPECT(val,exp) __builtin_expect((val),(exp))
#endif // UTIL_H_

