#ifndef MAESTRO_CONTROLLER_HPP
#define MAESTRO_CONTROLLER_HPP

#include "MaestroConfig.hpp"
#include "ServoController.hpp"
#include "Log.hpp"

#include <vector>

namespace shipcontrol
{

class MaestroController : public ServoController
{
public:
    MaestroController(MaestroConfig &config);
    ~MaestroController();
    SpeedVal get_speed();
    void set_speed(SpeedVal speed);
    SteeringVal get_steering();
    void set_steering(SteeringVal steering);

protected:
    MaestroConfig &_config;
    const char *_dev;
    std::vector<int> _engines;
    std::vector<int> _steering;
    int _fd;
    Log *_log;

    int speed_to_int(SpeedVal speed);
    SpeedVal int_to_speed(int speed);

    int steering_to_int(SteeringVal steering);
    SteeringVal int_to_steering(int steering);

    bool is_sane();
};

} // namespace shipcontrol

#endif // MAESTRO_CONTROLLER_HPP
