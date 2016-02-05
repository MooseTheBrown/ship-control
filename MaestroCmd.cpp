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

#include "MaestroCmd.hpp"
#include <unistd.h>
#include <cstring>

namespace shipcontrol
{

// creates command with up to 3 data bytes
MaestroCmd::MaestroCmd(int fd, MaestroCmdCode code,
                       unsigned char byte1,
                       unsigned char byte2,
                       unsigned char byte3) :
    _fd(fd)
{
    _cmd[0] = static_cast<unsigned char>(code);
    _cmd[1] = byte1;
    _cmd[2] = byte2;
    _cmd[3] = byte3;
    _log = Log::getInstance();
}

// execute command and get response
unsigned char *MaestroCmd::send()
{
    // calculate command length
    unsigned char len = cmdLen();

    std::memset((void *)_rsp, 0, MAESTRO_RSP_LEN);

    if (_fd == -1)
    {
        goto exit;
    }

    if (len < 1)
    {
        goto exit;
    }

    if (write(_fd, (const void *)_cmd, len) == -1)
    {
        _log->write(LogLevel::ERROR,
                   "MaestroCmd couldn't send command to Maestro, error code %d\n",
                   errno);
        goto exit;
    }
    _log->write(LogLevel::DEBUG, "MaestroCmd sent command: ");
    for (int i = 0; i < len; i++)
    {
        _log->write(LogLevel::DEBUG, "0x%x", _cmd[i]);
    }

    // calculate response length
    len = rspLen();
    if (len > 0)
    {
        if (read(_fd, (void *)_rsp, len) == -1)
        {
            _log->write(LogLevel::ERROR,
                       "MaestroCmd couldn't read response from Maestro, error code %d\n",
                       errno);
            goto exit;
        }
        _log->write(LogLevel::DEBUG, "MaestroCmd received response: ");
        for (int i = 0; i < len; i++)
        {
            _log->write(LogLevel::DEBUG, "0x%x ", _rsp[i]);
        }
    }

exit:
    return _rsp;
}

unsigned char MaestroCmd::cmdLen()
{
    switch (static_cast<MaestroCmdCode>(_cmd[0]))
    {
    case MaestroCmdCode::SETTARGET:
    case MaestroCmdCode::SETSPEED:
    case MaestroCmdCode::SETACCEL:
        return 4;
    case MaestroCmdCode::GETPOS:
        return 2;
    case MaestroCmdCode::GETMOVINGSTATE:
    case MaestroCmdCode::GETERRORS:
    case MaestroCmdCode::GOHOME:
        return 1;
    default:
        return 0;
    }
}

unsigned char MaestroCmd::rspLen()
{
    switch (static_cast<MaestroCmdCode>(_cmd[0]))
    {
    case MaestroCmdCode::SETTARGET:
    case MaestroCmdCode::SETSPEED:
    case MaestroCmdCode::SETACCEL:
    case MaestroCmdCode::GOHOME:
        return 0;
    case MaestroCmdCode::GETPOS:
    case MaestroCmdCode::GETERRORS:
        return 2;
    case MaestroCmdCode::GETMOVINGSTATE:
        return 1;
    default:
        return 0;
    }
}

} // namespace shipcontrol
