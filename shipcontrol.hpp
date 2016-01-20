#ifndef SHIPCONTROL_HPP
#define SHIPCONTROL_HPP

#include "Config.hpp"
#include "InputQueue.hpp"
#include "EvdevReader.hpp"
#include "MaestroController.hpp"
#include "SysLog.hpp"

namespace shipcontrol
{

#define CONFIG_FILE     "/etc/shipcontrol.conf"

#define RETVAL_OK               0
#define RETVAL_INVALID_CONFIG   1

#define PSMOVEINPUT_DEVICE_NAME     "psmoveinput"

class ShipControl
{
public:
    ShipControl();
    virtual ~ShipControl();

    int run();

protected:
    Config *_config;
    EvdevReader *_evdevReader;
    InputQueue _inputQueue;
    MaestroController *_controller;
    std::string _psmoveinput_dev;
    Log *_log;
    SysLog _syslog;

    int init();
    void find_input_device(const char *input_name, std::string &result);
    void turn_right();
    void turn_left();
    void speed_up();
    void speed_down();
    void setup_signals();
};

} // namespace shipcontrol

#endif // SHIPCONTROL_HPP
