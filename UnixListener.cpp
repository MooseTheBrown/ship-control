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

#include "UnixListener.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

namespace shipcontrol
{

UnixListener::UnixListener(IPCConfig &config, IPCRequestHandler &handler)
: _config(config),
  _fd(-1),
  _rq_handler(handler)
{
    _log = Log::getInstance();
    _socket_name = _config.get_unix_socket_name();
}

UnixListener::~UnixListener()
{
    Log::release();
    for (IPCClient *cl : _cl_handlers)
    {
        delete cl;
    }
}

void UnixListener::run()
{
    _log->write(LogLevel::DEBUG, "UnixListener::run()\n");

    if (setup() != true)
    {
        teardown();
        return;
    }

    while (true)
    {
        if (need_to_stop() == true)
        {
            break;
        }

        int clientsock = accept(_fd, nullptr, nullptr);
        if (clientsock == -1)
        {
            _log->write(LogLevel::ERROR, "UnixListener failed to accept connection, error code %d", errno);
            continue;
        }

        // handle data exchange in a separate client thread, here we just listen for new connections
        IPCClient *client = new IPCClient(clientsock, _rq_handler);
        _cl_handlers.push_back(client);
        client->start();
    }

    teardown();
}

bool UnixListener::setup()
{
    _log->write(LogLevel::DEBUG, "UnixListener::setup()\n");

    _fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_fd == -1)
    {
        _log->write(LogLevel::ERROR, "UnixListener failed to open socket, error code %d\n", errno);
        return false;
    }
    sockaddr_un addr;
    std::memset(&addr, 0, sizeof(sockaddr_un));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, _socket_name.c_str(), sizeof(addr.sun_path) - 1);

    _log->write(LogLevel::DEBUG, "UnixListener opening Unix socket %s\n", addr.sun_path);

    if (bind(_fd, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un)) == -1)
    {
        _log->write(LogLevel::ERROR, "UnixListener failed to bind socket, error code %d\n", errno);
        return false;
    }

    if (listen(_fd, 32) == -1)
    {
        _log->write(LogLevel::ERROR, "UnixListener failed to listen socket, error code %d\n", errno);
        return false;
    }

    return true;
}

void UnixListener::teardown()
{
    _log->write(LogLevel::DEBUG, "UnixListener::stop()\n");

    for (IPCClient *cl : _cl_handlers)
    {
        cl->stop();
    }

    if (_fd != -1)
    {
        close(_fd);
        _fd = -1;
        unlink(_socket_name.c_str());
    }
}

} // namespace shipcontrol
