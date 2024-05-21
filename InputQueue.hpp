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

#ifndef INPUT_QUEUE_HPP
#define INPUT_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

namespace shipcontrol
{

enum class InputEventType
{
    UNKNOWN,
    TURN_RIGHT,
    TURN_LEFT,
    SPEED_UP,
    SPEED_DOWN,
    SET_SPEED,
    SET_STEERING
};

struct InputEvent
{
    InputEventType type;
    std::string data;
};

// a thread-safe queue of input events
class InputQueue
{
public:
    InputQueue();
    virtual ~InputQueue();

    void push(InputEvent event);
    InputEvent pop();
    // pop_blocking blocks the calling thread until there's data in the queue
    InputEvent pop_blocking();
    bool is_empty();
protected:
    std::queue<InputEvent> _queue;
    std::mutex _queue_mutex;
    std::mutex _block_mutex;
    std::condition_variable _block_var;
};

} // namespace shipcontrol

#endif // INPUT_QUEUE_HPP
