/*
 * Copyright (C) 2016 - 2018 Mikhail Sapozhnikov
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

#ifndef EVDEV_READER_HPP
#define EVDEV_READER_HPP

#include "EvdevConfig.hpp"
#include "Log.hpp"
#include "SingleThread.hpp"
#include <linux/input.h>
#include <string>
#include <thread>

namespace shipcontrol
{

class EvdevReader : public SingleThread
{
public:
    EvdevReader(EvdevConfig &config,
                const std::string &dev,
                InputQueue &queue);
    virtual ~EvdevReader();

    virtual void run();
    virtual void stop();
protected:
    EvdevConfig &_config;
    std::string _dev;
    InputQueue &_queue;
    Log *_log;
    int _fd;
    const key_map *_keymap;
    const rel_map *_relmap;

    void handle_event(input_event &event);
    bool setup();
    void teardown();
};

} // namespace shipcontrol

#endif // EVDEV_READER_HPP
