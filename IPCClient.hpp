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

#ifndef IPCCLIENT_HPP
#define IPCCLIENT_HPP

#include "SingleThread.hpp"
#include "Log.hpp"
#include "IPCRequestHandler.hpp"

namespace shipcontrol
{

class IPCClient : public SingleThread
{
public:
    IPCClient(int fd, IPCRequestHandler &handler);
    IPCClient(const IPCClient &other) = delete;
    virtual ~IPCClient();

    virtual void run();
    virtual void stop();
protected:
    void teardown();

    const int BUFSIZE = 4096;

    int _fd;
    unsigned char *_buf;
    Log *_log;
    IPCRequestHandler &_rq_handler;
};

} // namespace shipcontrol

#endif // IPCCLIENT_HPP
