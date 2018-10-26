/*
 * Copyright (C) 2016 - 2018 Mikhail Sapozhnikov
 *
 * This file is part of ship-control.
 *
 * ship-control is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ship-control is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ship-control.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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

enum class LogBackendType : unsigned char
{
    CONSOLE = 0,
    SYSLOG = 1
};

// log interface
class Log
{
public:
    static Log *getInstance();
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
