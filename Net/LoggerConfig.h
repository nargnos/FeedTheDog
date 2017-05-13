#ifndef LOGGERCONFIG_H_
#define LOGGERCONFIG_H_
#include <syslog.h>
#include "Noncopyable.h"
enum LogPriority :int
{
	Emerg = LOG_EMERG,
	Alert = LOG_ALERT,
	Crit = LOG_CRIT,
	Err = LOG_ERR,
	Warning = LOG_WARNING,
	Notice = LOG_NOTICE,
	Info = LOG_WARNING,
	Debug = LOG_DEBUG,
};
// 比它小的都记录
#define PRIORITY_LEVEL LogPriority::Warning
#define APP_NAME "-hello world-"

class LoggerConfig :
	public Noncopyable
{
public:
	LoggerConfig();
	~LoggerConfig();
};
extern LoggerConfig cfg;

#endif // LOGGERCONFIG_H_

