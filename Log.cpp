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
        for (LogBackend *backend : _backends)
        {
            va_list args;
            va_start(args, fmt);
            backend->write(fmt, args);
            va_end(args);
        }
    }
}

void Log::write(const char *fmt, ...)
{
    if (LogLevel::NOTICE >= _level)
    {
        for (LogBackend *backend : _backends)
        {
            va_list args;
            va_start(args, fmt);
            backend->write(fmt, args);
            va_end(args);
        }
    }
}

} // namespace shipcontrol
