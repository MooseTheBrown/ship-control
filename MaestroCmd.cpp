#include "MaestroCmd.hpp"
#include <unistd.h>
#include <cstring>

namespace shipcontrol
{

// creates command with no data bytes
MaestroCmd::MaestroCmd(int fd, MaestroCmdCode code) :
    _fd(fd)
{
    _cmd[0] = code;
    _cmd[1] = 0;
    _cmd[2] = 0;
    _cmd[3] = 0;
    _log = Log::getInstance();
}

// creates command with up to 3 data bytes
MaestroCmd::MaestroCmd(int fd, MaestroCmdCode code,
                       unsigned char byte1 = 0,
                       unsigned char byte2 = 0,
                       unsigned char byte3 = 0) :
    _fd(fd)
{
    _cmd[0] = code;
    _cmd[1] = byte1;
    _cmd[2] = byte2;
    _cmd[3] = byte3;
    _log = Log::getInstance();
}

// execute command and get response
unsigned char *MaestroCmd::send()
{
    std::memset((void *)_rsp, 0, MAESTRO_RSP_LEN);

    if (_fd == -1)
    {
        goto exit;
    }

    // calculate command length
    unsigned char len = cmdLen();
    if (len < 1)
    {
        goto exit;
    }

    if (write(_fd, (const void *)_cmd, len) == -1)
    {
        _log.write(LogLevel::ERROR,
                   "MaestroCmd couldn't send command to Maestro, error code %d\n",
                   errno);
        goto exit;
    }
    _log.write(LogLevel::DEBUG, "MaestroCmd sent command: ");
    for (int i = 0; i < len; i++)
    {
        _log.write(LogLevel::DEBUG, "0x%x", _cmd[i]);
    }
    _log.write("\n");

    // calculate response length
    len = rspLen();
    if (len > 0)
    {
        if (read(_fd, (void *)_rsp, len) == -1)
        {
            _log.write(LogLevel::ERROR,
                       "MaestroCmd couldn't read response from Maestro, error code %d\n",
                       errno);
            goto exit;
        }
        _log.write(LogLevel::DEBUG, "MaestroCmd received response: ");
        for (int i = 0; i < len; i++)
        {
            _log.write(LogLevel::DEBUG, "0x%x", _rsp[i]);
        }
    }

exit:
    return _rsp;
}

unsigned char MaestroCmd::cmdLen()
{
    switch (_cmd[0])
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
    switch (_cmd[0])
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
