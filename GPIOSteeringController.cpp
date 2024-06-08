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

#include "GPIOSteeringController.hpp"
#include "GPIOUtil.hpp"

namespace shipcontrol
{

GPIOSteeringController::GPIOSteeringController(const GPIOSteeringConfig &config) :
_cur_steering(SteeringVal::STRAIGHT),
_pwm_thread(nullptr)
{
    _chip_path = config.chip_path;
    _steering_line = config.steering_line;
    _pwm_period = config.pwm_period;
    _min_duty_cycle = config.min_duty_cycle;
    _max_duty_cycle = config.max_duty_cycle;

    _log = Log::getInstance();

    if (config.syspwm_path.empty())
    {
        // S/W PWM mode
        _pwm_thread = new GPIOPWMThread(_chip_path, _steering_line, _pwm_period);
    }
    else
    {
        // H/W PWM mode
        _pwm_path = config.syspwm_path + "/pwm" + std::to_string(config.syspwm_num);
        GPIOUtil::sysfs_write(config.syspwm_path + "/export", std::to_string(config.syspwm_num), _log);
    }
}

GPIOSteeringController::~GPIOSteeringController()
{
    Log::release();

    delete _pwm_thread;
}

void GPIOSteeringController::start()
{
    if (_pwm_thread != nullptr)
    {
        _pwm_thread->start();
    }
    else
    {
        // HW PWM mode, set PWM period and duty cycle
        // Linux sysfs PWM API uses nanoseconds
        GPIOUtil::sysfs_write(_pwm_path + "/period", std::to_string(_pwm_period * 1000), _log);
        unsigned int straight = (_min_duty_cycle + _max_duty_cycle) / 2;
        unsigned int pwm_duration = straight * _pwm_period / 100;
        GPIOUtil::sysfs_write(_pwm_path + "/duty_cycle", std::to_string(pwm_duration * 1000), _log);
        // enable HW PWM
        GPIOUtil::sysfs_write(_pwm_path + "/enable", std::string("1"), _log);
    }
}

void GPIOSteeringController::stop()
{
    if (_pwm_thread != nullptr)
    {
        _pwm_thread->stop();
    }
    else
    {
        GPIOUtil::sysfs_write(_pwm_path + "/enable", std::string("0"), _log);
    }
}

void GPIOSteeringController::set_steering(SteeringVal steering)
{
    _cur_steering = steering;
    int int_steering = static_cast<int>(steering);
    // multiply duty cycle percentages by 10 to increase setting accuracy
    unsigned int min_duty_cycle = _min_duty_cycle * 10;
    unsigned int max_duty_cycle = _max_duty_cycle * 10;
    unsigned int straight = (min_duty_cycle + max_duty_cycle) / 2;
    unsigned int duty_cycle = straight + int_steering * (max_duty_cycle - straight) / 10;
    // divide by 1000 instead of 100 because we multiplied percentages by 10 earlier
    unsigned int pwm_duration = duty_cycle * _pwm_period / 1000;
    if (_pwm_thread != nullptr)
    {
        _pwm_thread->set_pwm_duration(pwm_duration);
    }
    else
    {
        GPIOUtil::sysfs_write(_pwm_path + "/duty_cycle", std::to_string(pwm_duration * 1000), _log);
    }
}

}