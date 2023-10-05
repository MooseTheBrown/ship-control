/* * Copyright (C) 2016 - 2023 Mikhail Sapozhnikov
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

#ifndef GPIO_CONFIG_HPP
#define GPIO_CONFIG_HPP

namespace shipcontrol
{

enum class GPIOReverseMode
{
    // reverse is not supported
    NO_REVERSE = 0,
    // reverse is managed by the same GPIO line as engine control
    SAME_LINE,
    // reverse is managed by the dedicated GPIO line
    DEDICATED_LINE
};

struct GPIOEngineConfig
{
    std::string chip_path;
    // engine line number for SW PWM
    unsigned int engine_line = 0;
    // sysfs path to pwm file for HW PWM; if not empty, HW PWM is used
    // example: /sys/class/pwm/pwmchip0
    std::string syspwm_path;
    // HW PWM line number. Combined with syspwm_path will give
    // full path to HW PWM, e.g. /sys/class/pwm/pwmchip0/pwm0
    unsigned int syspwm_num = 0;
    unsigned int dir_line = 0;
    unsigned int pwm_period = 0;
    GPIOReverseMode reverse_mode = GPIOReverseMode::NO_REVERSE;
};

} // namespace shipcontrol

#endif // GPIO_CONFIG_HPP 
