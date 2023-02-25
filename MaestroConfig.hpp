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

#ifndef MAESTRO_CONFIG_HPP
#define MAESTRO_CONFIG_HPP

#include <vector>

namespace shipcontrol
{

struct SteeringCalibration
{
    int straight;
    int step;
};

// engine config
struct MaestroEngine
{
    // rotation speed channel
    int channel;
    // rotation direction channel
    int dir_channel;
    // forward rotation flag
    bool fwd;
    // stop engine position
    int stop;
    // 10% speed increase/decrease step
    int step;

    static const int NO_CHANNEL;
};

// Maestro controller configuration provider
class MaestroConfig
{
public:
    static const int DEFAULT_DIR_HIGH;
    static const int DEFAULT_DIR_LOW;

    virtual const char *get_maestro_dev() = 0;
    virtual std::vector<MaestroEngine> get_engine_channels() = 0;
    virtual std::vector<int> get_steering_channels() = 0;
    virtual SteeringCalibration get_steering_calibration() = 0;
    virtual int get_direction_high() = 0;
    virtual int get_direction_low() = 0;
};

} // namespace shipcontrol

#endif // MAESTRO_CONFIG_HPP
