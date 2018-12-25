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

#ifndef MAESTRO_CONFIG_HPP
#define MAESTRO_CONFIG_HPP

#include <vector>

namespace shipcontrol
{

struct MaestroCalibration
{
    int max_fwd;
    int stop;
    int max_rev;
    int straight;
    int left_max;
    int right_max;
};

// Maestro controller configuration provider
class MaestroConfig
{
public:
    virtual const char *get_maestro_dev() = 0;
    virtual std::vector<int> get_engine_channels() = 0;
    virtual std::vector<int> get_steering_channels() = 0;
    virtual MaestroCalibration get_maestro_calibration() = 0;
};

} // namespace shipcontrol

#endif // MAESTRO_CONFIG_HPP
