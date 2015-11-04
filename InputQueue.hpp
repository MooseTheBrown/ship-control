#include <queue>
#include <mutex>

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
    bool is_empty();
protected:
    std::queue<InputEvent> _queue;
    std::mutex _queue_mutex;
};

} // namespace shipcontrol
