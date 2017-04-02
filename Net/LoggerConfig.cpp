#include "LoggerConfig.h"

LoggerConfig::LoggerConfig()
{
	openlog(APP_NAME, LOG_PID, LOG_USER);
	setlogmask(LOG_UPTO(PRIORITY_LEVEL));
}

LoggerConfig::~LoggerConfig()
{
	closelog();
}
LoggerConfig cfg;