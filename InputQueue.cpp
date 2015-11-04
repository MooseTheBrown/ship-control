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
    std::lock_guard<std::mutex> lock(_queue_mutex);
    _queue.push(event);
}

InputEvent InputQueue::pop()
{
    std::lock_guard<std::mutex> lock(_queue_mutex);
    InputEvent evt = _queue.front();
    _queue.pop();
    return evt;
}

bool InputQueue::is_empty()
{
    return _queue.empty();
}

} // namespace shipcontrol
