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

#include "EvdevReader.hpp"
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace shipcontrol
{

#define EAGAIN_TIMEOUT  100
#define EVENTS_AT_ONCE  64

EvdevReader::EvdevReader(EvdevConfig &config,
                         const std::string &dev,
                         InputQueue &queue)
: _config(config),
  _dev(dev),
  _queue(queue),
  _fd(-1)
{
    _log = Log::getInstance();
    _keymap = config.get_keymap();
    _relmap = config.get_relmap();
}

EvdevReader::~EvdevReader()
{
    Log::release();
}

void EvdevReader::run()
{
    if (setup() != true)
    {
        teardown();
        return;
    }

    while (true)
    {
        if (need_to_stop() == true)
        {
            break;
        }

        input_event events[EVENTS_AT_ONCE];
        ssize_t size = read(_fd, static_cast<void*>(events), sizeof (input_event) * EVENTS_AT_ONCE);
        if ((size < 0) || (size < sizeof (input_event)))
        {
            if (errno == EAGAIN)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(EAGAIN_TIMEOUT));
                continue;
            }
            else
            {
                _log->write(LogLevel::NOTICE, "EvdevReader failed to read, error code %d\n",
                            errno);
                teardown();
                return;
            }
        }
        for (int i = 0; i < (size / sizeof (input_event)); i++)
        {
            handle_event(events[i]);
        }
    }
}

void EvdevReader::stop()
{
    teardown();
    SingleThread::stop();
}

void EvdevReader::handle_event(input_event &event)
{
    if ((event.type == EV_KEY) && (event.value == 1))
    {
        auto match = _keymap->find(event.code);
        if (match != _keymap->end())
        {
            _queue.push(match->second);
        }
    }

    if ((event.type == EV_REL) && (event.value != 0))
    {
        RelEvent rel{event.code, ((event.value < 0) ? false : true)};
        auto match = _relmap->find(rel);
        if (match != _relmap->end())
        {
            _queue.push(match->second);
        }
    }
}

bool EvdevReader::setup()
{
    _fd = open(_dev.c_str(), O_RDONLY | O_NONBLOCK);

    if (_fd == -1)
    {
        _log->write(LogLevel::ERROR, "EvdevReader failed to open device %s, error code %d\n",
                    _dev.c_str(), errno);
        teardown();
        return false;
    }

    if (ioctl(_fd, EVIOCGRAB, (void*)1))
    {
        _log->write(LogLevel::ERROR, "EvdevReader failed to grab device %s, error code %d\n",
                    _dev.c_str(), errno);
        teardown();
        return false;
    }

    return true;
}

void EvdevReader::teardown()
{
    if (_fd != -1)
    {
        ioctl(_fd, EVIOCGRAB, (void*)0);
        close(_fd);
        _fd = -1;
    }
}

} // namespace shipcontrol
