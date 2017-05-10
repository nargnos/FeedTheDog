#ifndef LOGGER_H_
#define LOGGER_H_
#include <stddef.h>
#include <utility>
#include <memory>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <cassert>
#include "LoggerConfig.h"
#include "Noncopyable.h"

#define LOGSTR_ERRNO strerror(errno)
template<int Priority>
struct CanLog :
	public std::integral_constant<bool, Priority >= 0 && PRIORITY_LEVEL >= Priority>
{

};


// 可连写，但是分次记录，只共享优先级
// 使用如：TRACEPOINT(LogPriority::Debug)("%d hello", 1)("%d world", 2)("test");
template<int Priority>
struct WriteLog :
	public Noncopyable
{
	constexpr WriteLog() = default;
	template<typename... TArgs>
	inline const WriteLog& operator()(const char* fmt, TArgs&&... args)const
	{
		syslog(Priority, fmt, std::forward<TArgs>(args)...);
		return *this;
	}
	inline const WriteLog& Errno()const
	{
		return this->operator()(LOGSTR_ERRNO);
	}
	inline const WriteLog& Errno(const char* str)const
	{
		assert(str != nullptr);
		return this->operator()("%s : %s", str, LOGSTR_ERRNO);
	}
};

#define TRACEPOINT(Priority) if(CanLog<Priority>::value) WriteLog<Priority>()
#define TRACEPOINT_LINE(Priority) if(CanLog<Priority>::value) WriteLog<Priority>()("%s (%d)", __FILE__, __LINE__)
#define TRACEERRNO(Priority) TRACEPOINT(Priority).Errno()
#define TRACEERRNOSTR(Priority,str) TRACEPOINT(Priority).Errno(str)

#define TRACEPOINT_CODE(Priority) if(CanLog<Priority>::value) {
#define TRACEPOINT_ENDCODE }
#define EXIT_FAILURE 1
#define _EXIT _exit(EXIT_FAILURE);\
__builtin_unreachable(); 

#define TRACEERRNOEXIT(Priority) TRACEERRNO(Priority);\
_EXIT
#define TRACEERRNOEXITSTR(Priority,str) TRACEERRNOSTR(Priority,str);\
_EXIT



#endif // LOGGER_H_


