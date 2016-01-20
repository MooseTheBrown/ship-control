#ifndef INPUT_QUEUE_HPP
#define INPUT_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

namespace shipcontrol
{

enum class InputEvent
{
    UNKNOWN,
    TURN_RIGHT,
    TURN_LEFT,
    SPEED_UP,
    SPEED_DOWN
};

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
