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

#ifndef GPIOSTEERINGCONFIG_HPP
#define GPIOSTEERINGCONFIG_HPP

#include <string>

namespace shipcontrol
{

struct GPIOSteeringConfig
{
    std::string chip_path;
    unsigned int steering_line = 0;
    // sysfs path to pwm file for HW PWM; if not empty, HW PWM is used
    // example: /sys/class/pwm/pwmchip0
    std::string syspwm_path;
    // HW PWM line number. Combined with syspwm_path will give
    // full path to HW PWM, e.g. /sys/class/pwm/pwmchip0/pwm0
    unsigned int syspwm_num = 0;
    // pwm period in microseonds
    unsigned int pwm_period = 0;
    // min and max duty cycle are in % of pwm_period
    unsigned int min_duty_cycle = 10;
    unsigned int max_duty_cycle = 20;
};

}

#endif // GPIOSTEERINGCONFIG_HPP