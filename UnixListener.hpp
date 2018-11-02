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

#ifndef UNIXLISTENER_HPP
#define UNIXLISTENER_HPP

#include "IPCConfig.hpp"
#include "Log.hpp"
#include "SingleThread.hpp"
#include "IPCRequestHandler.hpp"
#include "IPCClient.hpp"
#include <vector>

namespace shipcontrol
{

class UnixListener : public SingleThread
{
public:
    UnixListener(IPCConfig &config, IPCRequestHandler &handler);
    virtual ~UnixListener();

    virtual void run();
protected:
    bool setup();
    void teardown();

    Log *_log;
    IPCConfig& _config;
    std::string _socket_name;
    int _fd;
    IPCRequestHandler &_rq_handler;
    std::vector<IPCClient *> _cl_handlers;
};

} // namespace shipcontrol

#endif // UNIXLISTENER_HPP
