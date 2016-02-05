/*
 * Copyright (C) 2016 Mikhail Sapozhnikov
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

#include "InputQueue.hpp"

namespace shipcontrol
{

InputQueue::InputQueue()
{
}

InputQueue::~InputQueue()
{
}

void InputQueue::push(InputEvent event)
{
    {
        std::lock_guard<std::mutex> queue_lock(_queue_mutex);
        std::lock_guard<std::mutex> block_lock(_block_mutex);
        _queue.push(event);
    }
    _block_var.notify_all();
}

InputEvent InputQueue::pop()
{
    std::lock_guard<std::mutex> lock(_queue_mutex);
    InputEvent evt = _queue.front();
    _queue.pop();
    return evt;
}

InputEvent InputQueue::pop_blocking()
{
    std::unique_lock<std::mutex> lock(_block_mutex);
    _block_var.wait(lock, [this](){ return !is_empty(); });
    lock.unlock();
    return pop();
}

bool InputQueue::is_empty()
{
    return _queue.empty();
}

} // namespace shipcontrol
