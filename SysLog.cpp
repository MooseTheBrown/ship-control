#include "SysLog.hpp"
#include <syslog.h>

namespace shipcontrol
{

const char *SysLog::_ident = "ship-control";

SysLog::SysLog()
{
    openlog(_ident, LOG_CONS, LOG_USER);
}

SysLog::~SysLog()
{
    closelog();
}

void SysLog::write(const char *fmt, va_list args)
{
    vsyslog(LOG_INFO, fmt, args);
}

} // namespace shipcontrol
