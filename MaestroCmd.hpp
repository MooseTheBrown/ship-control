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

#ifndef MAESTRO_CMD_HPP
#define MAESTRO_CMD_HPP

#include "Log.hpp"

namespace shipcontrol
{

// Maestro command codes
enum class MaestroCmdCode : unsigned char
{
    SETTARGET = 0x84,
    SETSPEED = 0x87,
    SETACCEL = 0x89,
    GETPOS = 0x90,
    GETMOVINGSTATE = 0x93,
    GETERRORS = 0xA1,
    GOHOME = 0xA2
};

// maximum number of bytes in Maestro command
#define MAESTRO_CMD_LEN     4
// maximum number of bytes in Maestro response
#define MAESTRO_RSP_LEN     2

class MaestroCmd
{
public:
    // creates command with up to 3 data bytes
    MaestroCmd(int fd, MaestroCmdCode code,
               unsigned char byte1 = 0,
               unsigned char byte2 = 0,
               unsigned char byte3 = 0);
    // execute command and get response
    unsigned char *send();

protected:
    unsigned char cmdLen();
    unsigned char rspLen();

    int _fd;
    unsigned char _cmd[MAESTRO_CMD_LEN];
    unsigned char _rsp[MAESTRO_RSP_LEN];
    Log *_log;
};

} // namespace shipcontrol

#endif // MAESTRO_CMD_HPP
