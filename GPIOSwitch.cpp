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

#include "GPIOSwitch.hpp"

#include <exception>

namespace shipcontrol
{

GPIOSwitch::GPIOSwitch(const std::string &chip_path, unsigned int line_num) :
    _chip_path(chip_path),
    _line_num(line_num),
    _line_init(false),
    _ok(false)
{
    _log = Log::getInstance();
    _log->write(LogLevel::DEBUG, "GPIOSwitch ctor, chip_path=%s, line_num=%d\n",
            chip_path.c_str(), line_num);

    try
    {
        _chip = new gpiod::chip(chip_path);
        _line = _chip->get_line(line_num);
        _line_init = true;
        _line.request({"shipcontrol::GPIOPWMThread",
                gpiod::line_request::DIRECTION_OUTPUT,
                0},
                0);
        _ok = true;
    }
    catch (const std::exception &e)
    {
        _log->write(LogLevel::ERROR, "GPIOSwitch initialization failed: %s\n",
                    e.what());
    }
}

GPIOSwitch::~GPIOSwitch()
{
    try
    {
        if (_line_init == true)
        {
            _line.release();
        }
        if (_chip != nullptr)
        {
            delete _chip;
        }
    }
    catch (const std::exception &e) {}
    Log::release();
}

void GPIOSwitch::on()
{
    _log->write(LogLevel::DEBUG, "GPIOSwitch line %d ON", _line_num);
    _line.set_value(1);
}

void GPIOSwitch::off()
{
    _log->write(LogLevel::DEBUG, "GPIOSwitch line %d OFF", _line_num);
    _line.set_value(0);
}

} // namespace shipcontrol
