#include "EvdevConfig.hpp"

namespace shipcontrol
{

// RelEvent implementation
bool RelEvent::operator ==(const RelEvent &other) const
{
    if ((axis == other.axis) && (positive == other.positive))
    {
        return true;
    }
    return false;
}

bool RelEvent::operator !=(const RelEvent &other) const
{
    if ((axis != other.axis) || (positive != other.positive))
    {
        return true;
    }
    return false;
}

std::size_t RelEventHasher::operator ()(const RelEvent &evt) const
{
    return (std::hash<int>()(evt.axis) ^ std::hash<bool>()(evt.positive));
}

} // namespace shipcontrol
