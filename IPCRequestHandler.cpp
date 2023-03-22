/*
 * Copyright (C) 2016 - 2023 Mikhail Sapozhnikov
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

#include <stdexcept>
#include "IPCRequestHandler.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace shipcontrol
{

IPCRequestHandler::IPCRequestHandler(InputQueue &queue, DataProvider &provider)
: _input_queue(queue),
  _data_provider(provider)
{
}

std::string IPCRequestHandler::handleRequest(const std::string &request)
{
    try
    {
        json json_rq = json::parse(request);

        if (json_rq.find("type") != json_rq.end())
        {
            std::string rq_type = json_rq["type"].get<std::string>();
            if (rq_type == "cmd")
            {
                if (json_rq.find("cmd") != json_rq.end())
                {
                    std::string cmd = json_rq["cmd"].get<std::string>();
                    std::string data;
                    if ((cmd == "set_speed") || (cmd == "set_steering"))
                    {
                        if (json_rq.find("data") != json_rq.end())
                        {
                            data = json_rq["data"].get<std::string>();
                        }
                        else
                        {
                            throw std::invalid_argument("no data for set_speed or set_steering command");
                        }
                    }
                    return handle_cmd(cmd, data);
                }
                else
                {
                    throw std::invalid_argument("no command inside command request");
                }
            }
            else if (rq_type == "query")
            {
                return handle_query();
            }
            else
            {
                throw std::invalid_argument("invalid request type");
            }
        }
        else
        {
            throw std::invalid_argument("invalid request message, request type not found");
        }
    }
    catch (const std::exception &e)
    {
        json json_resp;

        json_resp["status"] = "fail";
        json_resp["error"] = e.what();

        return json_resp.dump();
    }
}

std::string IPCRequestHandler::handle_cmd(const std::string &cmd, const std::string &data)
{
    InputEvent evt;
    evt.type = InputEventType::UNKNOWN;
    json json_resp;

    if (cmd == "speed_up")
    {
        evt.type = InputEventType::SPEED_UP;
    }
    else if (cmd == "speed_down")
    {
        evt.type = InputEventType::SPEED_DOWN;
    }
    else if (cmd == "turn_left")
    {
        evt.type = InputEventType::TURN_LEFT;
    }
    else if (cmd == "turn_right")
    {
        evt.type = InputEventType::TURN_RIGHT;
    }
    else if (cmd == "set_speed")
    {
        if (ServoController::validate_speed_str(data) == true)
        {
            evt.type = InputEventType::SET_SPEED;
            evt.data = data;
        }
        else
        {
            evt.type = InputEventType::UNKNOWN;
        }
    }
    else if (cmd == "set_steering")
    {
        if (ServoController::validate_steering_str(data) == true)
        {
            evt.type = InputEventType::SET_STEERING;
            evt.data = data;
        }
        else
        {
            evt.type = InputEventType::UNKNOWN;
        }
    }

    if (evt.type != InputEventType::UNKNOWN)
    {
        _input_queue.push(evt);
        json_resp["status"] = "ok";
        json_resp["error"] = "";
    }
    else
    {
        json_resp["status"] = "fail";
        json_resp["error"] = "invalid command";
    }

    return json_resp.dump();
}

std::string IPCRequestHandler::handle_query()
{
    json j;

    j["speed"] = ServoController::speed_to_str(_data_provider.get_speed());
    j["steering"] = ServoController::steering_to_str(_data_provider.get_steering());

    return j.dump();
}

} // namespace shipcontrol
