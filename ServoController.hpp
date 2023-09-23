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

#ifndef SERVO_CONTROLLER_HPP
#define SERVO_CONTROLLER_HPP

#include <unordered_map>
#include <unordered_set>
#include <string>

namespace shipcontrol
{

// speed values in % of maximum
enum class SpeedVal
{
    STOP = 0,
    FWD10 = 1,
    FWD20 = 2,
    FWD30 = 3,
    FWD40 = 4,
    FWD50 = 5,
    FWD60 = 6,
    FWD70 = 7,
    FWD80 = 8,
    FWD90 = 9,
    FWD100 = 10,
    REV10 = -1,
    REV20 = -2,
    REV30 = -3,
    REV40 = -4,
    REV50 = -5,
    REV60 = -6,
    REV70 = -7,
    REV80 = -8,
    REV90 = -9,
    REV100 = -10
};

// steering values in % of maximum angle
enum class SteeringVal
{
    STRAIGHT = 0,
    RIGHT10 = 1,
    RIGHT20 = 2,
    RIGHT30 = 3,
    RIGHT40 = 4,
    RIGHT50 = 5,
    RIGHT60 = 6,
    RIGHT70 = 7,
    RIGHT80 = 8,
    RIGHT90 = 9,
    RIGHT100 = 10,
    LEFT10 = -1,
    LEFT20 = -2,
    LEFT30 = -3,
    LEFT40 = -4,
    LEFT50 = -5,
    LEFT60 = -6,
    LEFT70 = -7,
    LEFT80 = -8,
    LEFT90 = -9,
    LEFT100 = -10
};

// servo controller interface
class ServoController
{
public:
    virtual ~ServoController() {}

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual SpeedVal get_speed() = 0;
    virtual void set_speed(SpeedVal speed) = 0;
    virtual SteeringVal get_steering() = 0;
    virtual void set_steering(SteeringVal steering) = 0;

    static std::string speed_to_str(SpeedVal speed);
    static SpeedVal str_to_speed(const std::string &str);

    static std::string steering_to_str(SteeringVal steering);
    static SteeringVal str_to_steering(const std::string &str);

    static bool validate_speed_str(const std::string &str);
    static bool validate_steering_str(const std::string &str);

private:
    static std::unordered_map<SpeedVal, std::string> _speed_map;
    static std::unordered_map<SteeringVal, std::string> _steering_map;
    static std::unordered_set<std::string> _speed_values;
    static std::unordered_set<std::string> _steering_values;

    static std::unordered_set<std::string> init_speed_values();
    static std::unordered_set<std::string> init_steering_values();
};

} // namespace shipcontrol

#endif // SERVO_CONTROLLER_HPP
