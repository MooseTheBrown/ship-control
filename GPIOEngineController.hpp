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

#ifndef GPIO_CONTROLLER_HPP
#define GPIO_CONTROLLER_HPP

#include <string>

#include <gpiod.hpp>

#include "ServoController.hpp"
#include "Log.hpp"
#include "GPIOEngineConfig.hpp"
#include "GPIOPWMThread.hpp"

namespace shipcontrol
{

// controller of a single PWM-over-GPIO-driven engine
class GPIOEngineController : public ServoController
{
public:
    GPIOEngineController(const GPIOEngineConfig &config);
    virtual ~GPIOEngineController();

    // ServoController implementation
    SpeedVal get_speed();
    void set_speed(SpeedVal speed);
    SteeringVal get_steering();
    void set_steering(SteeringVal steering);

    virtual void start();
    virtual void stop();

protected:
    // GPIO device path
    std::string _chip_path;
    // engine line number
    unsigned int _engine_line_num;
    // direction line number
    unsigned int _dir_line_num;
    // engine PWM period - microseconds between consecutive high level signals
    unsigned int _pwm_period;
    GPIOReverseMode _rev_mode;
    // current engine speed
    SpeedVal _cur_speed;

    GPIOPWMThread *_pwm_thread;

    gpiod::chip *_gpio_chip;
    gpiod::line _dir_line;

    Log *_log;
};

} // namespace shipcontrol

#endif // GPIO_CONTROLLER_HPP
