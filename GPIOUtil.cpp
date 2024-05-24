/*
 * Copyright (C) 2016 - 2024 Mikhail Sapozhnikov
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

#include "GPIOUtil.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace shipcontrol
{

void GPIOUtil::sysfs_write(const std::string &path, const std::string &value, Log *log)
{
    log->write(LogLevel::DEBUG, "GPIOEngineController::sysfs_write(), path=%s, value=%s\n",
            path.c_str(), value.c_str());

    int fd = open(path.c_str(), O_WRONLY);

    if (fd == -1)
    {
        log->write(LogLevel::ERROR,
                "GPIOEngineController failed to open file %s, errno=%d\n",
                path.c_str(), errno);
        return;
    }

    ssize_t written = write(fd, reinterpret_cast<const void*>(value.c_str()), value.size());
    if (written != value.size())
    {
        log->write(LogLevel::ERROR,
                "GPIOEngineController write to %s, expected to write %d bytes, wrote %d instead, errno=%d\n",
                path.c_str(), value.size(), written, errno);
    }

    close(fd);
}
}