#ifndef MAESTRO_CONFIG_HPP
#define MAESTRO_CONFIG_HPP

#include <vector>

namespace shipcontrol
{

// Maestro controller configuration provider
class MaestroConfig
{
public:
    virtual const char *get_maestro_dev() = 0;
    virtual std::vector<int> get_engine_channels() = 0;
    virtual std::vector<int> get_steering_channels() = 0;
};

}

#endif // MAESTRO_CONFIG_HPP
