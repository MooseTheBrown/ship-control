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

#include "IPCClient.hpp"
#include <unistd.h>

namespace shipcontrol
{

IPCClient::IPCClient(int fd, IPCRequestHandler &handler)
: _fd(fd),
  _rq_handler(handler)
{
    _buf = static_cast<unsigned char *>(new unsigned char[BUFSIZE]);
    _log = Log::getInstance();
}

IPCClient::~IPCClient()
{
    Log::release();
    delete _buf;
}

void IPCClient::run()
{
    if (_fd == -1)
    {
        return;
    }

    while (true)
    {
        if (need_to_stop() == true)
        {
            break;
        }

        // read request, pass it to IPCRequestHandler for processing and send back the response
        int len = read(_fd, reinterpret_cast<void *>(_buf), BUFSIZE);
        if (len == -1)
        {
            _log->write(LogLevel::NOTICE, "IPCClient failed to read data from socket, error code %d", errno);
            break;
        }
        _buf[len] = '\0';
        std::string resp = _rq_handler.handleRequest(std::string(reinterpret_cast<char *>(_buf)));

        if (write(_fd, reinterpret_cast<const void *>(resp.c_str()), resp.length()) == -1)
        {
            _log->write(LogLevel::NOTICE, "IPCClient failed to write data into socket, error code %d", errno);
            break;
        }
    }
}

void IPCClient::stop()
{
    teardown();
    SingleThread::stop();
}

void IPCClient::teardown()
{
    if (_fd != -1)
    {
        close(_fd);
    }
}

} // namespace shipcontrol
