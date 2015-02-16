#include "Log.hpp"

namespace shipcontrol
{

unsigned int Log::_refs = 0;
std::mutex Log::_ref_mutex;
Log *Log::_instance = nullptr;

Log *Log::getInstance()
{
    std::lock_guard<std::mutex> lock(_ref_mutex);

    if (_refs == 0)
    {
        _instance = new Log();
    }
    _refs++;

    return _instance;
}

void Log::release()
{
    std::lock_guard<std::mutex> lock(_ref_mutex);

    _refs--;
    if (_refs == 0)
    {
        delete _instance;
        _instance = nullptr;
    }
}

Log::Log() : _level(LogLevel::NOTICE)
{
}

Log::~Log()
{
}


void Log::add_backend(LogBackend *backend)
{
    if (backend != nullptr)
    {
        _backends.push_back(backend);
    }
}

void Log::write(LogLevel level, const char *fmt, ...)
{
    if (level >= _level)
    {
        va_list args;
        va_start(args, fmt);
        for (LogBackend *backend : _backends)
        {
            backend->write(fmt, args);
        }
        va_end(args);
    }
}

void Log::write(const char *fmt, ...)
{
    if (LogLevel::NOTICE >= _level)
    {
        va_list args;
        va_start(args, fmt);
        for (LogBackend *backend : _backends)
        {
            backend->write(fmt, args);
        }
        va_end(args);
    }
}

} // namespace shipcontrol
