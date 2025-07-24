/*
 * Copyright (C) 2016 - 2025 Mikhail Sapozhnikov
 *
 * This file is part of ship-control.
 *
 * ship-control is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ship-control is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ship-control.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SHIPCONTROL_HPP
#define SHIPCONTROL_HPP

#include <vector>

#include "Config.hpp"
#include "InputQueue.hpp"
#include "EvdevReader.hpp"
#include "MaestroController.hpp"
#include "ConsoleLog.hpp"
#include "SysLog.hpp"
#include "DataProvider.hpp"
#include "UnixListener.hpp"
#include "GPIOSwitch.hpp"

namespace shipcontrol
{

#define CONFIG_FILE     "/etc/shipcontrol.conf"

#define RETVAL_OK               0
#define RETVAL_INVALID_CONFIG   1
#define RETVAL_INVALID_CMDLINE  2

#define PSMOVEINPUT_DEVICE_NAME     "psmoveinput"

// ship-control mode of operation
enum class ShipControlMode
{
    // normal mode, run event processing loop
    NORMAL = 0,
    // execute given commands, wait for arbitrary user input and exit
    COMMAND,
    // print help message and exit
    HELP
};

class ShipControl : public DataProvider
{
public:
    ShipControl();
    virtual ~ShipControl();

    int run(int argc, char **argv);
    void interrupt();

    // DataProvider implementation
    virtual SpeedVal get_speed() { return _speed; }
    virtual SteeringVal get_steering() { return _steering; }

protected:
    Config *_config;
    EvdevReader *_evdevReader;
    InputQueue _inputQueue;
    std::string _psmoveinput_dev;
    Log *_log;
    ConsoleLog _clog;
    SysLog _syslog;
    SpeedVal _speed;
    SteeringVal _steering;
    IPCRequestHandler *_ipcHandler;
    UnixListener *_unixListener;
    bool _stop;
    std::vector<ServoController*> _servo_controllers;
    ShipControlMode _mode;
    // used in command mode only
    std::string _cmd_speed;
    std::string _cmd_steering;
    GPIOSwitch *_water_cooling_switch;

    int handle_cmd_line(int argc, char **argv);
    int init();
    void find_input_device(const char *input_name, std::string &result);
    void turn_right();
    void turn_left();
    void speed_up();
    void speed_down();
    void set_speed(const std::string &speed_str);
    void set_steering(const std::string &steering_str);
    void setup_signals();
    void set_water_cooling(SpeedVal speed);
};

} // namespace shipcontrol

#endif // SHIPCONTROL_HPP
