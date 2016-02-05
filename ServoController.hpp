/*
 * Copyright (C) 2016 Mikhail Sapozhnikov
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

namespace shipcontrol
{

// speed values in % of maximum
enum class SpeedVal
{
    STOP,
    FWD10,
    FWD20,
    FWD30,
    FWD40,
    FWD50,
    FWD60,
    FWD70,
    FWD80,
    FWD90,
    FWD100,
    REV10,
    REV20,
    REV30,
    REV40,
    REV50,
    REV60,
    REV70,
    REV80,
    REV90,
    REV100
};

// steering values in % of maximum angle
enum class SteeringVal
{
    STRAIGHT,
    RIGHT10,
    RIGHT20,
    RIGHT30,
    RIGHT40,
    RIGHT50,
    RIGHT60,
    RIGHT70,
    RIGHT80,
    RIGHT90,
    RIGHT100,
    LEFT10,
    LEFT20,
    LEFT30,
    LEFT40,
    LEFT50,
    LEFT60,
    LEFT70,
    LEFT80,
    LEFT90,
    LEFT100
};

// servo controller interface
class ServoController
{
public:
    virtual SpeedVal get_speed() = 0;
    virtual void set_speed(SpeedVal speed) = 0;
    virtual SteeringVal get_steering() = 0;
    virtual void set_steering(SteeringVal steering) = 0;
};

} // namespace shipcontrol

#endif // SERVO_CONTROLLER_HPP
