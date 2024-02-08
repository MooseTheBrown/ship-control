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

#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "GPIOEngineController.hpp"

namespace shipcontrol
{

GPIOEngineController::GPIOEngineController(const GPIOEngineConfig &config) :
    _cur_speed(SpeedVal::STOP),
    _gpio_chip(nullptr),
    _syspwm_path("")
{
    _chip_path = config.chip_path;
    _engine_line_num = config.engine_line;
    _dir_line_num = config.dir_line;
    _pwm_period = config.pwm_period;
    _min_duty_cycle = config.min_duty_cycle;
    _max_duty_cycle = config.max_duty_cycle;
    _rev_mode = config.reverse_mode;

    _log = Log::getInstance();

    _log->write(LogLevel::DEBUG, "GPIOEngineController ctor, _chip_path=%s, _engine_line_num=%d, _dir_line_num=%d, _pwm_period=%d, _rev_mode=%d\n",
            _chip_path.c_str(), _engine_line_num, _dir_line_num,
            _pwm_period, static_cast<int>(_rev_mode));

    if (config.syspwm_path != "")
    {
        // HW PWM mode
        _pwm_thread = nullptr;
        _syspwm_path = config.syspwm_path + "/pwm" + std::to_string(config.syspwm_num);
        sysfs_write(config.syspwm_path + "/export", std::to_string(config.syspwm_num));
    }
    else
    {
        // SW PWM mode
        _pwm_thread = new GPIOPWMThread(_chip_path, _engine_line_num, _pwm_period);
    }

    if (_rev_mode == GPIOReverseMode::DEDICATED_LINE)
    {
        try
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
        catch (const std::exception &e)
        {
            _log->write(LogLevel::ERROR, "GPIOEngineController caught exception: %s\n", e.what());
        }
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
            unsigned int neutral = (_max_duty_cycle - _min_duty_cycle) / 2;
            unsigned int step = (_max_duty_cycle - neutral) / 10;
            int duty_cycle = neutral + step * int_speed;
            if (duty_cycle < _min_duty_cycle)
            {
                duty_cycle = _min_duty_cycle;
            }
            else if (duty_cycle > _max_duty_cycle)
            {
                duty_cycle = _max_duty_cycle;
            }

            unsigned int pwm_duration = duty_cycle * _pwm_period / 100;

            if (_pwm_thread != nullptr)
            {
                _pwm_thread->set_pwm_duration(static_cast<unsigned int>(pwm_duration));
            }
            else
            {
                sysfs_write(_syspwm_path + "/duty_cycle", std::to_string(pwm_duration * 1000));
            }
            break;
        }

        case GPIOReverseMode::DEDICATED_LINE:
        {
            if (int_speed >= 0)
            {
                _dir_line.set_value(1);
                _log->write(LogLevel::DEBUG,
                        "GPIOEngineController, set direction line to 1\n");
            }
            else
            {
                _dir_line.set_value(0);
                _log->write(LogLevel::DEBUG,
                        "GPIOEngineController, set direction line to 0\n");
            }
            // fall through is intentional
        }

        case GPIOReverseMode::NO_REVERSE:
        {
            int_speed = std::abs(int_speed);
            unsigned int duty_cycle = _min_duty_cycle + (int_speed * (_max_duty_cycle - _min_duty_cycle) / 10 );
            unsigned int pwm_duration = duty_cycle * _pwm_period / 100;
            if (_pwm_thread != nullptr)
            {
                _pwm_thread->set_pwm_duration(static_cast<unsigned int>(pwm_duration));
            }
            else
            {
                sysfs_write(_syspwm_path + "/duty_cycle", std::to_string(pwm_duration * 1000));
            }

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
    if (_pwm_thread != nullptr)
    {
        // SW PWM mode
        _pwm_thread->start();
    }
    else
    {
        // HW PWM mode, set PWM period and duty cycle
        // Linux sysfs PWM API uses nanoseconds
        sysfs_write(_syspwm_path + "/period", std::to_string(_pwm_period * 1000));
        unsigned int pwm_duration = _min_duty_cycle * _pwm_period / 100;
        sysfs_write(_syspwm_path + "/duty_cycle", std::to_string(pwm_duration * 1000));
        // enable HW PWM
        sysfs_write(_syspwm_path + "/enable", std::string("1"));
    }
}

void GPIOEngineController::stop()
{
    if (_pwm_thread != nullptr)
    {
        _pwm_thread->stop();
    }
    else
    {
        // disable HW PWM
        sysfs_write(_syspwm_path + "/enable", std::string("0"));
    }
}

void GPIOEngineController::sysfs_write(const std::string &path, const std::string &value)
{
    _log->write(LogLevel::DEBUG, "GPIOEngineController::sysfs_write(), path=%s, value=%s\n",
            path.c_str(), value.c_str());

    int fd = open(path.c_str(), O_WRONLY);

    if (fd == -1)
    {
        _log->write(LogLevel::ERROR,
                "GPIOEngineController failed to open file %s, errno=%d\n",
                path.c_str(), errno);
        return;
    }

    ssize_t written = write(fd, reinterpret_cast<const void*>(value.c_str()), value.size());
    if (written != value.size())
    {
        _log->write(LogLevel::ERROR,
                "GPIOEngineController write to %s, expected to write %d bytes, wrote %d instead, errno=%d\n",
                path.c_str(), value.size(), written, errno);
    }

    close(fd);
}

} // namespace shipcontrol
