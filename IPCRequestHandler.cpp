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
    json json_rq = json::parse(request);

    try {
        if (json_rq.find("type") != json_rq.end())
        {
            std::string rq_type = json_rq["type"].get<std::string>();
            if (rq_type == "cmd")
            {
                if (json_rq.find("cmd") != json_rq.end())
                {
                    std::string cmd = json_rq["cmd"].get<std::string>();
                    return handle_cmd(cmd);
                }
                else
                {
                    throw std::string("no command inside command request");
                }
            }
            else if (rq_type == "query")
            {
                return handle_query();
            }
            else
            {
                throw std::string("invalid request type");
            }
        }
        else
        {
            throw std::string("invalid request message, request type not found");
        }
    }
    catch (const std::string &e)
    {
        json json_resp;

        json_resp["status"] = "fail";
        json_resp["error"] = e;

        return json_resp.dump();
    }
}

std::string IPCRequestHandler::handle_cmd(const std::string &cmd)
{
    InputEvent evt = InputEvent::UNKNOWN;
    json json_resp;

    if (cmd == "speed_up")
    {
        evt = InputEvent::SPEED_UP;
    }
    else if (cmd == "speed_down")
    {
        evt = InputEvent::SPEED_DOWN;
    }
    else if (cmd == "turn_left")
    {
        evt = InputEvent::TURN_LEFT;
    }
    else if (cmd == "turn_right")
    {
        evt = InputEvent::TURN_RIGHT;
    }

    if (evt != InputEvent::UNKNOWN)
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

    j["speed"] = speed_to_str(_data_provider.get_speed());
    j["steering"] = steering_to_str(_data_provider.get_steering());

    return j.dump();
}

std::string IPCRequestHandler::speed_to_str(SpeedVal speed)
{
    switch (speed)
    {
    case SpeedVal::STOP:
        return std::string("stop");
    case SpeedVal::FWD10:
        return std::string("fwd10");
    case SpeedVal::FWD20:
        return std::string("fwd20");
    case SpeedVal::FWD30:
        return std::string("fwd30");
    case SpeedVal::FWD40:
        return std::string("fwd40");
    case SpeedVal::FWD50:
        return std::string("fwd50");
    case SpeedVal::FWD60:
        return std::string("fwd60");
    case SpeedVal::FWD70:
        return std::string("fwd70");
    case SpeedVal::FWD80:
        return std::string("fwd80");
    case SpeedVal::FWD90:
        return std::string("fwd90");
    case SpeedVal::FWD100:
        return std::string("fwd100");
    case SpeedVal::REV10:
        return std::string("rev10");
    case SpeedVal::REV20:
        return std::string("rev20");
    case SpeedVal::REV30:
        return std::string("rev30");
    case SpeedVal::REV40:
        return std::string("rev40");
    case SpeedVal::REV50:
        return std::string("rev50");
    case SpeedVal::REV60:
        return std::string("rev60");
    case SpeedVal::REV70:
        return std::string("rev70");
    case SpeedVal::REV80:
        return std::string("rev80");
    case SpeedVal::REV90:
        return std::string("rev90");
    case SpeedVal::REV100:
        return std::string("rev100");
    default:
        return std::string("");
    }
}

std::string IPCRequestHandler::steering_to_str(SteeringVal steering)
{
    switch (steering)
    {
    case SteeringVal::STRAIGHT:
        return std::string("straight");
    case SteeringVal::RIGHT10:
        return std::string("right10");
    case SteeringVal::RIGHT20:
        return std::string("right20");
    case SteeringVal::RIGHT30:
        return std::string("right30");
    case SteeringVal::RIGHT40:
        return std::string("right40");
    case SteeringVal::RIGHT50:
        return std::string("right50");
    case SteeringVal::RIGHT60:
        return std::string("right60");
    case SteeringVal::RIGHT70:
        return std::string("right70");
    case SteeringVal::RIGHT80:
        return std::string("right80");
    case SteeringVal::RIGHT90:
        return std::string("right90");
    case SteeringVal::RIGHT100:
        return std::string("right100");
    case SteeringVal::LEFT10:
        return std::string("left10");
    case SteeringVal::LEFT20:
        return std::string("left20");
    case SteeringVal::LEFT30:
        return std::string("left30");
    case SteeringVal::LEFT40:
        return std::string("left40");
    case SteeringVal::LEFT50:
        return std::string("left50");
    case SteeringVal::LEFT60:
        return std::string("left60");
    case SteeringVal::LEFT70:
        return std::string("left70");
    case SteeringVal::LEFT80:
        return std::string("left80");
    case SteeringVal::LEFT90:
        return std::string("left90");
    case SteeringVal::LEFT100:
        return std::string("left100");
    default:
        return std::string("");
    }
}

} // namespace shipcontrol
