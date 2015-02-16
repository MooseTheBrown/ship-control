#include "Log.hpp"

namespace shipcontrol
{

// log backend, which writes to syslog
class SysLog : public LogBackend
{
public:
    SysLog();
    virtual ~SysLog();
    virtual void write(const char *fmt, va_list args);

protected:
    static const char *_ident;
};

} // namespace shipcontrol
