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
