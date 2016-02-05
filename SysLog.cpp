/*
 * Copyright (C) 2016 Mikhail Sapozhnikov
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
