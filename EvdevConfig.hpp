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
