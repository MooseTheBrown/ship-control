#ifndef EVDEV_READER_HPP
#define EVDEV_READER_HPP

#include "EvdevConfig.hpp"
#include "Log.hpp"
#include <string>
#include <thread>
#include <linux/input.h>

namespace shipcontrol
{

class EvdevReader
{
public:
    EvdevReader(EvdevConfig &config,
                const std::string &dev,
                InputQueue &queue);
    virtual ~EvdevReader();

    void start();
    void stop();

protected:
    EvdevConfig &_config;
    std::string _dev;
    InputQueue &_queue;
    std::thread *_thread;
    Log *_log;
    int _fd;
    bool _need_to_stop;
    const key_map *_keymap;
    const rel_map *_relmap;

    void run();
    void handle_event(input_event &event);
    void cleanup();
};

} // namespace shipcontrol

#endif // EVDEV_READER_HPP
