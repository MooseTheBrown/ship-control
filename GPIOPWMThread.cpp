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

#include <thread>
#include <chrono>
#include <exception>

#include <gpiod.hpp>

#include "GPIOPWMThread.hpp"

namespace shipcontrol
{

GPIOPWMThread::GPIOPWMThread(const std::string &chip_path,
                             unsigned int engine_line,
                             unsigned int pwm_period) :
    _chip_path(chip_path),
    _engine_line(engine_line),
    _pwm_period(pwm_period),
    _pwm_duration(0)
{
    _log = Log::getInstance();
    _log->write(LogLevel::DEBUG, "GPIOPWMThread ctor, chip_path=%s, engine_line=%d, pwm_period=%d\n",
            chip_path.c_str(), engine_line, pwm_period);
}

GPIOPWMThread::~GPIOPWMThread()
{
    Log::release();
}

void GPIOPWMThread::run()
{
    _log->write(LogLevel::DEBUG, "GPIOPWMThread::run()\n");

    try
    {
        gpiod::chip chip(_chip_path);
        gpiod::line line = chip.get_line(_engine_line);
        line.request({"shipcontrol::GPIOPWMThread",
                gpiod::line_request::DIRECTION_OUTPUT,
                0},
                0);

        while (true)
        {
            if (need_to_stop() == true)
            {
                _log->write(LogLevel::DEBUG, "GPIOPWMThread stopping\n");
                break;
            }

            unsigned int cur_pwm_duration = _pwm_duration;

            if (cur_pwm_duration != 0)
            {
                // set GPIO high level and wait for PWM duration
                line.set_value(1);
                std::this_thread::sleep_for(std::chrono::microseconds(cur_pwm_duration));
            }

            // set GPIO low level and wait for the next iteration
            line.set_value(0);
            std::this_thread::sleep_for(std::chrono::microseconds(_pwm_period - cur_pwm_duration));
        }

        line.release();
    }
    catch (const std::exception &e)
    {
        _log->write(LogLevel::ERROR, "GPIOPWMThread caught exception: %s\n", e.what());
    }
}

void GPIOPWMThread::set_pwm_duration(unsigned int pwm_duration)
{
    _pwm_duration = pwm_duration;
    if (_pwm_duration >= _pwm_period)
    {
        _pwm_duration = _pwm_period - 1;
    }
    _log->write(LogLevel::DEBUG, "GPIOPWMThread::set_pwm_duration(%d), new duration = %d\n",
           pwm_duration, _pwm_duration);
}

} // namespace shipcontrol
