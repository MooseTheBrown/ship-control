/*
 * Copyright (C) 2016 - 2025 Mikhail Sapozhnikov
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

#ifndef GPIOSWITCH_HPP
#define GPIOSWITCH_HPP

#include <string>

#include <gpiod.hpp>

#include "Log.hpp"

namespace shipcontrol
{

class GPIOSwitch
{
public:
    GPIOSwitch(const std::string &chip_path,
               unsigned int line_num);
    virtual ~GPIOSwitch();

    void on();
    void off();
    bool is_ok() { return _ok; }

protected:
    const std::string _chip_path;
    const unsigned int _line_num;
    gpiod::chip *_chip;
    gpiod::line _line;
    bool _line_init;
    bool _ok;
    Log *_log;
};

} // namespace shipcontrol

#endif // GPIOSWITCH_HPP
