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

// log interface
class Log
{
public:
    static Log &getInstance();
    static void release();
    void add_backend(LogBackend *backend);
    void write(const char *fmt, ...);

protected:
    Log();
    virtual ~Log();

    static unsigned int _refs;
    static std::mutex _ref_mutex;
    static Log *_instance;

    std::vector<LogBackend *> _backends;
};

} // namespace shipcontrol

#endif // LOG_HPP
