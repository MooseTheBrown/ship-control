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
    unsigned int engine_line;
    unsigned int dir_line;
    unsigned int pwm_period;
    GPIOReverseMode reverse_mode;
};

} // namespace shipcontrol

#endif // GPIO_CONFIG_HPP 
