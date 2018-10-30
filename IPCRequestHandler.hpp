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

#ifndef IPCREQUESTHANDLER_HPP
#define IPCREQUESTHANDLER_HPP

#include "InputQueue.hpp"
#include "DataProvider.hpp"
#include <string>

namespace shipcontrol
{

/*
 * IPC request format:
 * JSON
 * {
 *     "type": "cmd" or "query",
 *     "cmd" (in case of cmd type): "speed_up" or "speed_down" or "turn_left" or "turn_right"
 * }
 *
 * IPC command response format:
 * JSON
 * {
 *     "status": "ok" or "fail",
 *     "error": "<error_message>" in case of status=fail
 * }
 * This is sent in case of failed query as well
 *
 * IPC query response format:
 * JSON
 * {
 *     "speed": "<value>",
 *     "steering": "<value>"
 * }
 */

class IPCRequestHandler
{
public:
    IPCRequestHandler(InputQueue &queue, DataProvider &provider);
    std::string handleRequest(const std::string &request);
protected:
    std::string handle_cmd(const std::string &cmd);
    std::string handle_query();
    std::string speed_to_str(SpeedVal speed);
    std::string steering_to_str(SteeringVal steering);

    InputQueue &_input_queue;
    DataProvider &_data_provider;
};

} // namespace shipcontrol

#endif // IPCREQUESTHANDLER_HPP
