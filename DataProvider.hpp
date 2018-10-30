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

#ifndef DATAPROVIDER_HPP
#define DATAPROVIDER_HPP

#include "ServoController.hpp" // included for SpeedVal/SteeringVal definitions

namespace shipcontrol
{

/*
 * Data provider interface.
 * Unlike ServoController, this interface doesn't give any control over
 * engine/steering operation, it simply provides current ship status
 */
class DataProvider
{
public:
    virtual SpeedVal get_speed() = 0;
    virtual SteeringVal get_steering() = 0;
};

} // namespace shipcontrol

#endif // DATAPROVIDER_HPP
