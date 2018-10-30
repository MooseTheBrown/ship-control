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

#ifndef CONSOLELOG_HPP
#define CONSOLELOG_HPP

#include "Log.hpp"

namespace shipcontrol
{

// log backend, which writes to console
class ConsoleLog : public LogBackend
{
public:
    ConsoleLog();
    virtual ~ConsoleLog();
    virtual void write(const char *fmt, va_list args);
};

} // namespace shipcontrol

#endif // CONSOLELOG_HPP
