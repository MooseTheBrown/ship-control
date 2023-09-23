/*
 * Copyright (C) 2016 - 2023 Mikhail Sapozhnikov
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

namespace shipcontrol
{

#define CONFIG_FILE     "/etc/shipcontrol.conf"

#define RETVAL_OK               0
#define RETVAL_INVALID_CONFIG   1

#define PSMOVEINPUT_DEVICE_NAME     "psmoveinput"

class ShipControl : public DataProvider
{
public:
    ShipControl(int argc, char **argv);
    virtual ~ShipControl();

    int run();
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

    int init();
    void find_input_device(const char *input_name, std::string &result);
    void turn_right();
    void turn_left();
    void speed_up();
    void speed_down();
    void set_speed(const std::string &speed_str);
    void set_steering(const std::string &steering_str);
    void setup_signals();
};

} // namespace shipcontrol

#endif // SHIPCONTROL_HPP
