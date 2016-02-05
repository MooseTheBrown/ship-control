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

#ifndef EVDEV_CONFIG_HPP
#define EVDEV_CONFIG_HPP

#include "InputQueue.hpp"
#include <unordered_map>

namespace shipcontrol
{

struct RelEvent
{
    int axis;
    bool positive;

    bool operator ==(const RelEvent &other) const;
    bool operator !=(const RelEvent &other) const;
};

class RelEventHasher
{
public:
    std::size_t operator ()(const RelEvent &evt) const;
};

// KEY event to shipcontrol action mapping
typedef std::unordered_map<int, InputEvent> key_map;
// REL event to shipcontrol action mapping
typedef std::unordered_map<RelEvent, InputEvent, RelEventHasher> rel_map;

// Configuration provider for EvdevReader
class EvdevConfig
{
public:
    // get KEY_ event mapping
    virtual const key_map *get_keymap() = 0;
    // get REL_ event mapping
    virtual const rel_map *get_relmap() = 0;
};

} // namespace shipcontrol

#endif // EVDEV_CONFIG_HPP
