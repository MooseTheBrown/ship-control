/*
 * Copyright (C) 2016 - 2024 Mikhail Sapozhnikov
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

#ifndef GPIOSTEERINGCONTROLLER_HPP
#define GPIOSTEERINGCONTROLLER_HPP

#include "ServoController.hpp"
#include "GPIOSteeringConfig.hpp"
#include "GPIOPWMThread.hpp"
#include "Log.hpp"

namespace shipcontrol
{

class GPIOSteeringController : public ServoController
{
public:
    GPIOSteeringController(const GPIOSteeringConfig &config);
    virtual ~GPIOSteeringController();

    // ServoController implementation
    virtual SpeedVal get_speed() { return SpeedVal::STOP; }
    virtual void set_speed(SpeedVal speed) { /* N/A*/ }
    virtual SteeringVal get_steering() { return _cur_steering; }
    virtual void set_steering(SteeringVal steering);

    virtual void start();
    virtual void stop();

protected:
    std::string _chip_path;
    unsigned int _steering_line;
    std::string _pwm_path;
    unsigned int _pwm_period;
    unsigned int _min_duty_cycle;
    unsigned int _max_duty_cycle;
    SteeringVal _cur_steering;

    Log *_log;

    GPIOPWMThread *_pwm_thread;
};

}

#endif // GPIOSTEERINGCONTROLLER_HPP