#ifndef LOG_HPP
#define LOG_HPP

#include <mutex>
#include <vector>
#include <cstdarg>

namespace shipcontrol
{

// log backend interface
class LogBackend
{
public:
    virtual ~LogBackend() {}
    virtual void write(const char *fmt, va_list args) = 0;
};

enum class LogLevel : unsigned char
{
    ERROR = 2,
    NOTICE = 1,
    DEBUG = 0
};

// log interface
class Log
{
public:
    static Log &getInstance();
    static void release();
    void add_backend(LogBackend *backend);
    void write(LogLevel level, const char *fmt, ...);
    // write message with default log level - notice
    void write(const char *fmt, ...);
    void set_level(LogLevel level) { _level = level; }

protected:
    Log();
    virtual ~Log();

    static unsigned int _refs;
    static std::mutex _ref_mutex;
    static Log *_instance;

    std::vector<LogBackend *> _backends;
    LogLevel _level;
};

} // namespace shipcontrol

#endif // LOG_HPP
