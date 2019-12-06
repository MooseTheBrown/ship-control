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

#ifndef MAESTRO_CONTROLLER_HPP
#define MAESTRO_CONTROLLER_HPP

#include "MaestroConfig.hpp"
#include "ServoController.hpp"
#include "Log.hpp"

#include <vector>

namespace shipcontrol
{

class MaestroController : public ServoController
{
public:
    MaestroController(MaestroConfig &config);
    ~MaestroController();
    SpeedVal get_speed();
    void set_speed(SpeedVal speed);
    SteeringVal get_steering();
    void set_steering(SteeringVal steering);

protected:
    MaestroConfig &_config;
    const char *_dev;
    std::vector<MaestroEngine> _engines;
    std::vector<int> _steering;
    MaestroCalibration _calibration;
    int _fwd_range;
    int _rev_range;
    int _right_range;
    int _left_range;
    int _fd;
    Log *_log;

    int speed_to_int(SpeedVal speed);
    SpeedVal int_to_speed(int speed);
    SpeedVal mirror_speed(SpeedVal speed);

    int steering_to_int(SteeringVal steering);
    SteeringVal int_to_steering(int steering);

    bool is_sane();
};

} // namespace shipcontrol

#endif // MAESTRO_CONTROLLER_HPP
