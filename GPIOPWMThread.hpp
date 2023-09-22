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

#ifndef GPIO_PWM_THREAD_HPP
#define GPIO_PWM_THREAD_HPP

#include <string>

#include "Log.hpp"
#include "SingleThread.hpp"

namespace shipcontrol
{

class GPIOPWMThread : public SingleThread
{
public:
    GPIOPWMThread(const std::string &chip,
                  unsigned int engine_line,
                  unsigned int pwm_period);
    virtual ~GPIOPWMThread();

    virtual void run();

    void set_pwm_duration(unsigned int pwm_duration);

protected:
    const std::string _chip_path;
    const unsigned int _engine_line;
    const unsigned int _pwm_period;
    unsigned int _pwm_duration;
    Log *_log;
};

} // namespace shipcontrol

#endif // GPIO_PWM_THREAD_HPP
