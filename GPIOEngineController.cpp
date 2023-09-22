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

#include <cstdlib>

#include "GPIOEngineController.hpp"

namespace shipcontrol
{

GPIOEngineController::GPIOEngineController(const GPIOEngineConfig &config) :
    _cur_speed(SpeedVal::STOP),
    _gpio_chip(nullptr)
{
    _chip_path = config.chip_path;
    _engine_line_num = config.engine_line;
    _dir_line_num = config.dir_line;
    _pwm_period = config.pwm_period;
    _rev_mode = config.reverse_mode;

    _log = Log::getInstance();

    _log->write(LogLevel::DEBUG, "GPIOEngineController ctor, _chip_path=%s, _engine_line_num=%d, _dir_line_num=%d, _pwm_period=%d, _rev_mode=%d\n",
            _chip_path, _engine_line_num, _dir_line_num,
            _pwm_period, static_cast<int>(_rev_mode));

    _pwm_thread = new GPIOPWMThread(_chip_path, _engine_line_num, _pwm_period);

    if (_rev_mode == GPIOReverseMode::DEDICATED_LINE)
    {
        _gpio_chip = new gpiod::chip(_chip_path); 
        if (!_gpio_chip)
        {
            _log->write(LogLevel::ERROR,
                    "GPIOEngineController failed to open gpio chip at path %s\n",
                    _chip_path);
            return;
        }

        _dir_line = _gpio_chip->get_line(_dir_line_num);
        _dir_line.request({"shipcontrol::GPIOEngineController",
               gpiod::line_request::DIRECTION_OUTPUT,
               0},
               0);
    }
}

GPIOEngineController::~GPIOEngineController()
{
    Log::release();
    if (_pwm_thread != nullptr)
    {
        delete _pwm_thread;
    }
    if (_gpio_chip != nullptr)
    {
        delete _gpio_chip;
    }
}

void GPIOEngineController::set_steering(SteeringVal steering)
{
    // N/A
}

SteeringVal GPIOEngineController::get_steering()
{
    return SteeringVal::STRAIGHT;
}

void GPIOEngineController::set_speed(SpeedVal speed)
{
    _cur_speed = speed;
    int int_speed = static_cast<int>(_cur_speed);

    switch (_rev_mode)
    {
        case GPIOReverseMode::SAME_LINE:
        {
            unsigned int neutral = _pwm_period / 2;
            unsigned int step = _pwm_period - neutral / 10;
            int new_pwm_duration = neutral + step * int_speed;
            if (new_pwm_duration < 0)
            {
                new_pwm_duration = 0;
            }
            else if (new_pwm_duration > _pwm_period)
            {
                new_pwm_duration = _pwm_period;
            }
            _pwm_thread->set_pwm_duration(static_cast<unsigned int>(new_pwm_duration));
            break;
        }

        case GPIOReverseMode::DEDICATED_LINE:
        {
            if (int_speed >= 0)
            {
                _dir_line.set_value(1);
                _log->write(LogLevel::DEBUG,
                        "GPIOEngineController, set direction line to 1");
            }
            else
            {
                _dir_line.set_value(0);
                _log->write(LogLevel::DEBUG,
                        "GPIOEngineController, set direction line to 0");
            }
            // fall through is intentional
        }

        case GPIOReverseMode::NO_REVERSE:
        {
            int_speed = std::abs(int_speed);
            _pwm_thread->set_pwm_duration((_pwm_period / 10) * int_speed);
            break;
        }
    }

}

SpeedVal GPIOEngineController::get_speed()
{
    return _cur_speed;
}

void GPIOEngineController::start()
{
    _pwm_thread->start();
}

void GPIOEngineController::stop()
{
    _pwm_thread->stop();
}

} // namespace shipcontrol
