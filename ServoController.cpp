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

#include "ServoController.hpp"
#include <stdexcept>

namespace shipcontrol
{

std::unordered_map<SpeedVal, std::string> ServoController::_speed_map = {
    {SpeedVal::STOP, "stop"},
    {SpeedVal::FWD10, "fwd10"},
    {SpeedVal::FWD20, "fwd20"},
    {SpeedVal::FWD30, "fwd30"},
    {SpeedVal::FWD40, "fwd40"},
    {SpeedVal::FWD50, "fwd50"},
    {SpeedVal::FWD60, "fwd60"},
    {SpeedVal::FWD70, "fwd70"},
    {SpeedVal::FWD80, "fwd80"},
    {SpeedVal::FWD90, "fwd90"},
    {SpeedVal::FWD100, "fwd100"},
    {SpeedVal::REV10, "rev10"},
    {SpeedVal::REV20, "rev20"},
    {SpeedVal::REV30, "rev30"},
    {SpeedVal::REV40, "rev40"},
    {SpeedVal::REV50, "rev50"},
    {SpeedVal::REV60, "rev60"},
    {SpeedVal::REV70, "rev70"},
    {SpeedVal::REV80, "rev80"},
    {SpeedVal::REV90, "rev90"},
    {SpeedVal::REV100, "rev100"}
};

std::unordered_map<SteeringVal, std::string> ServoController::_steering_map = {
    {SteeringVal::STRAIGHT, "straight"},
    {SteeringVal::RIGHT10, "right10"},
    {SteeringVal::RIGHT20, "right20"},
    {SteeringVal::RIGHT30, "right30"},
    {SteeringVal::RIGHT40, "right40"},
    {SteeringVal::RIGHT50, "right50"},
    {SteeringVal::RIGHT60, "right60"},
    {SteeringVal::RIGHT70, "right70"},
    {SteeringVal::RIGHT80, "right80"},
    {SteeringVal::RIGHT90, "right90"},
    {SteeringVal::RIGHT100, "right100"},
    {SteeringVal::LEFT10, "left10"},
    {SteeringVal::LEFT20, "left20"},
    {SteeringVal::LEFT30, "left30"},
    {SteeringVal::LEFT40, "left40"},
    {SteeringVal::LEFT50, "left50"},
    {SteeringVal::LEFT60, "left60"},
    {SteeringVal::LEFT70, "left70"},
    {SteeringVal::LEFT80, "left80"},
    {SteeringVal::LEFT90, "left90"},
    {SteeringVal::LEFT100, "left100"}
};

std::unordered_set<std::string> ServoController::_speed_values = std::move(init_speed_values());
std::unordered_set<std::string> ServoController::_steering_values = std::move(init_steering_values());

std::unordered_set<std::string> ServoController::init_speed_values()
{
    std::unordered_set<std::string> speed_set;
    for (const std::pair<SpeedVal, std::string> &item : _speed_map)
    {
        speed_set.insert(item.second);
    }
    return std::move(speed_set);
}

std::unordered_set<std::string> ServoController::init_steering_values()
{
    std::unordered_set<std::string> steering_set;
    for (const std::pair<SteeringVal, std::string> &item : _steering_map)
    {
        steering_set.insert(item.second);
    }
    return std::move(steering_set);
}

std::string ServoController::speed_to_str(SpeedVal speed)
{
    try
    {
        return _speed_map.at(speed);
    }
    catch (const std::out_of_range &e)
    {
        return std::string("");
    }
}

SpeedVal ServoController::str_to_speed(const std::string &str)
{
    for (const std::pair<SpeedVal, std::string> &item : _speed_map)
    {
        if (item.second == str)
        {
            return item.first;
        }
    }

    return SpeedVal::STOP;
}

std::string ServoController::steering_to_str(SteeringVal steering)
{
    try
    {
        return _steering_map.at(steering);
    }
    catch (const std::out_of_range &e)
    {
        return std::string("");
    }
}

SteeringVal ServoController::str_to_steering(const std::string &str)
{
    for (const std::pair<SteeringVal, std::string> &item : _steering_map)
    {
        if (item.second == str)
        {
            return item.first;
        }
    }

    return SteeringVal::STRAIGHT;
}

bool ServoController::validate_speed_str(const std::string &str)
{
    if (_speed_values.find(str) != _speed_values.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ServoController::validate_steering_str(const std::string &str)
{
    if (_steering_values.find(str) != _steering_values.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace shipcontrol
