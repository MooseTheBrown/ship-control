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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include <boost/program_options.hpp>

#include "shipcontrol.hpp"
#include "GPIOEngineController.hpp"
#include "GPIOSteeringController.hpp"
#include "GPIOSwitchConfig.hpp"

extern void signal_handler(int sig);

namespace po = boost::program_options;

namespace shipcontrol
{

ShipControl::ShipControl() :
    _config(nullptr),
    _evdevReader(nullptr),
    _speed(SpeedVal::STOP),
    _steering(SteeringVal::STRAIGHT),
    _ipcHandler(nullptr),
    _unixListener(nullptr),
    _stop(false),
    _mode(ShipControlMode::NORMAL),
    _cmd_speed(""),
    _cmd_steering(""),
    _water_cooling_switch(nullptr)
{
    _log = Log::getInstance();
}

ShipControl::~ShipControl()
{
    if (_config != nullptr)
    {
        delete _config;
    }
    if (_evdevReader != nullptr)
    {
        delete _evdevReader;
    }
    if (_ipcHandler != nullptr)
    {
        delete _ipcHandler;
    }
    if (_unixListener != nullptr)
    {
        delete _unixListener;
    }
    for (ServoController *controller : _servo_controllers)
    {
        if (controller != nullptr)
        {
            delete controller;
        }
    }
    if (_water_cooling_switch != nullptr)
    {
        delete _water_cooling_switch;
    }
    Log::release();
}

int ShipControl::handle_cmd_line(int argc, char **argv)
{
    try
    {
        po::variables_map opts;
        po::options_description opt_descr("Available options:");

        opt_descr.add_options()
            ("help", "print help message")
            ("speed", po::value<std::string>(), "set speed")
            ("steering", po::value<std::string>(), "set steering");

        po::store(po::parse_command_line(argc, argv, opt_descr), opts);
        po::notify(opts);

        if (opts.count("help"))
        {
            _mode = ShipControlMode::HELP;
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << opt_descr << "\n";
            return RETVAL_OK;
        }

        if (opts.count("speed"))
        {
            _cmd_speed = opts["speed"].as<std::string>();
            _mode = ShipControlMode::COMMAND;
        }

        if (opts.count("steering"))
        {
            _cmd_steering = opts["steering"].as<std::string>();
            _mode = ShipControlMode::COMMAND;
        }

        return RETVAL_OK;
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << "\n";
        return RETVAL_INVALID_CMDLINE;
    }
}

int ShipControl::run(int argc, char **argv)
{
    int ret = handle_cmd_line(argc, argv);
    if (ret != RETVAL_OK)
    {
        return ret;
    }

    if (_mode == ShipControlMode::HELP)
    {
        // help message has already been printed, just quit
        return RETVAL_OK;
    }

    ret = init();

    if (ret != RETVAL_OK)
    {
        return ret;
    }

    _evdevReader->start();
    _unixListener->start();

    for (ServoController *controller : _servo_controllers)
    {
        controller->start();
    }

    if (_mode == ShipControlMode::NORMAL)
    {
        // event handling loop
        while (true)
        {
            if (_stop)
            {
                break;
            }

            InputEvent evt = _inputQueue.pop_blocking();
            switch (evt.type)
            {
            case InputEventType::TURN_RIGHT:
                turn_right();
                break;
            case InputEventType::TURN_LEFT:
                turn_left();
                break;
            case InputEventType::SPEED_UP:
                speed_up();
                break;
            case InputEventType::SPEED_DOWN:
                speed_down();
                break;
            case InputEventType::SET_SPEED:
                set_speed(evt.data);
                break;
            case InputEventType::SET_STEERING:
                set_steering(evt.data);
                break;
            default:
                break;
            }
        }
    }
    else
    {
        // command mode
        set_speed(_cmd_speed);
        set_steering(_cmd_steering);
        std::cout << "Press any key to exit\n";
        std::cin.get();
    }

    for (ServoController *controller : _servo_controllers)
    {
        controller->stop();
    }

    _evdevReader->stop();
    _unixListener->stop();

    return RETVAL_OK;
}

int ShipControl::init()
{
    // read config
    _config = new Config(CONFIG_FILE);
    if (_config->is_ok() == false)
    {
        return RETVAL_INVALID_CONFIG;
    }

    setup_signals();

    // configure logging
    std::vector<LogBackendType> log_backends = _config->get_log_backends();
    for (auto backend : log_backends)
    {
        if (backend == LogBackendType::CONSOLE)
        {
            _log->add_backend(&_clog);
        }
        else if (backend == LogBackendType::SYSLOG)
        {
            _log->add_backend(&_syslog);
        }
    }
    _log->set_level(_config->get_log_level());

    // initialize input
    find_input_device(PSMOVEINPUT_DEVICE_NAME, _psmoveinput_dev);
    _evdevReader = new EvdevReader(*_config, _psmoveinput_dev, _inputQueue);

    // initialize Maestro controller
    MaestroController *maestro_controller = new MaestroController(*_config);
    _servo_controllers.push_back(maestro_controller);

    // initialize GPIO engine controllers
    std::vector<GPIOEngineConfig> gpio_engine_configs = _config->get_gpio_engine_configs();
    for (GPIOEngineConfig gpio_engine_config : gpio_engine_configs)
    {
        GPIOEngineController *gpio_engine_controller = new GPIOEngineController(gpio_engine_config);
        _servo_controllers.push_back(gpio_engine_controller);
    }

    // initialize GPIO steering controllers
    std::vector<GPIOSteeringConfig> gpio_steering_configs  =  _config->get_gpio_steering_configs();
    for (GPIOSteeringConfig gpio_steering_config  : gpio_steering_configs)
    {
        GPIOSteeringController *gpio_steering_controller  = new GPIOSteeringController(gpio_steering_config);
        _servo_controllers.push_back(gpio_steering_controller);
    }

    // initialize water cooling switch
    GPIOSwitchConfig *wc_config = _config->get_water_cooling_relay_config();
    if (wc_config != nullptr)
    {
        _water_cooling_switch = new GPIOSwitch(wc_config->chip_path, wc_config->line_num);
    }

    // initialize Unix socket listener
    _ipcHandler = new IPCRequestHandler(_inputQueue, *this);
    _unixListener = new UnixListener(*_config, *_ipcHandler);

    return RETVAL_OK;
}

void ShipControl::interrupt()
{
    _stop = true;
    // push dummy event to the input queue, so that run() could detect stop flag
    _inputQueue.push(InputEvent{InputEventType::UNKNOWN, ""});
}

void ShipControl::find_input_device(const char *input_name, std::string &result)
{
    dirent *entry;
    DIR *dir = opendir("/dev/input");
    if (dir == nullptr)
    {
        return;
    }

    while ((entry = readdir(dir)) != nullptr)
    {
        std::string filename = std::string("/dev/input/") + std::string(entry->d_name);
        int fd = open(filename.c_str(), O_RDONLY);
        char name[256];
        std::memset(name, 0, sizeof (name));
        ioctl(fd, EVIOCGNAME(sizeof (name)), name);
        if (std::strncmp(name, input_name, sizeof (name)) == 0)
        {
            result = std::move(filename);
            _log->write(LogLevel::DEBUG, "found input device %s at %s\n",
                        input_name, result.c_str());
            close(fd);
            break;
        }
        close(fd);
    }

    closedir(dir);
}

void ShipControl::turn_right()
{
    SteeringVal new_steering;

    switch (_steering)
    {
    case SteeringVal::STRAIGHT:
        new_steering = SteeringVal::RIGHT10;
        break;
    case SteeringVal::RIGHT10:
        new_steering = SteeringVal::RIGHT20;
        break;
    case SteeringVal::RIGHT20:
        new_steering = SteeringVal::RIGHT30;
        break;
    case SteeringVal::RIGHT30:
        new_steering = SteeringVal::RIGHT40;
        break;
    case SteeringVal::RIGHT40:
        new_steering = SteeringVal::RIGHT50;
        break;
    case SteeringVal::RIGHT50:
        new_steering = SteeringVal::RIGHT60;
        break;
    case SteeringVal::RIGHT60:
        new_steering = SteeringVal::RIGHT70;
        break;
    case SteeringVal::RIGHT70:
        new_steering = SteeringVal::RIGHT80;
        break;
    case SteeringVal::RIGHT80:
        new_steering = SteeringVal::RIGHT90;
        break;
    case SteeringVal::RIGHT90:
        new_steering = SteeringVal::RIGHT100;
        break;
    case SteeringVal::LEFT10:
        new_steering = SteeringVal::STRAIGHT;
        break;
    case SteeringVal::LEFT20:
        new_steering = SteeringVal::LEFT10;
        break;
    case SteeringVal::LEFT30:
        new_steering = SteeringVal::LEFT20;
        break;
    case SteeringVal::LEFT40:
        new_steering = SteeringVal::LEFT30;
        break;
    case SteeringVal::LEFT50:
        new_steering = SteeringVal::LEFT40;
        break;
    case SteeringVal::LEFT60:
        new_steering = SteeringVal::LEFT50;
        break;
    case SteeringVal::LEFT70:
        new_steering = SteeringVal::LEFT60;
        break;
    case SteeringVal::LEFT80:
        new_steering = SteeringVal::LEFT70;
        break;
    case SteeringVal::LEFT90:
        new_steering = SteeringVal::LEFT80;
        break;
    case SteeringVal::LEFT100:
        new_steering = SteeringVal::LEFT90;
        break;
    default:
        return;
    }

    for (ServoController *controller : _servo_controllers)
    {
        controller->set_steering(new_steering);
    }
    _steering = new_steering;
}

void ShipControl::turn_left()
{
    SteeringVal new_steering;

    switch (_steering)
    {
    case SteeringVal::STRAIGHT:
        new_steering = SteeringVal::LEFT10;
        break;
    case SteeringVal::RIGHT10:
        new_steering = SteeringVal::STRAIGHT;
        break;
    case SteeringVal::RIGHT20:
        new_steering = SteeringVal::RIGHT10;
        break;
    case SteeringVal::RIGHT30:
        new_steering = SteeringVal::RIGHT20;
        break;
    case SteeringVal::RIGHT40:
        new_steering = SteeringVal::RIGHT30;
        break;
    case SteeringVal::RIGHT50:
        new_steering = SteeringVal::RIGHT40;
        break;
    case SteeringVal::RIGHT60:
        new_steering = SteeringVal::RIGHT50;
        break;
    case SteeringVal::RIGHT70:
        new_steering = SteeringVal::RIGHT60;
        break;
    case SteeringVal::RIGHT80:
        new_steering = SteeringVal::RIGHT70;
        break;
    case SteeringVal::RIGHT90:
        new_steering = SteeringVal::RIGHT80;
        break;
    case SteeringVal::RIGHT100:
        new_steering = SteeringVal::RIGHT90;
        break;
    case SteeringVal::LEFT10:
        new_steering = SteeringVal::LEFT20;
        break;
    case SteeringVal::LEFT20:
        new_steering = SteeringVal::LEFT30;
        break;
    case SteeringVal::LEFT30:
        new_steering = SteeringVal::LEFT40;
        break;
    case SteeringVal::LEFT40:
        new_steering = SteeringVal::LEFT50;
        break;
    case SteeringVal::LEFT50:
        new_steering = SteeringVal::LEFT60;
        break;
    case SteeringVal::LEFT60:
        new_steering = SteeringVal::LEFT70;
        break;
    case SteeringVal::LEFT70:
        new_steering = SteeringVal::LEFT80;
        break;
    case SteeringVal::LEFT80:
        new_steering = SteeringVal::LEFT90;
        break;
    case SteeringVal::LEFT90:
        new_steering = SteeringVal::LEFT100;
        break;
    default:
        return;
    }

    for (ServoController *controller : _servo_controllers)
    {
        controller->set_steering(new_steering);
    }
    _steering = new_steering;
}

void ShipControl::speed_up()
{
    SpeedVal new_speed;

    switch (_speed)
    {
    case SpeedVal::STOP:
        new_speed = SpeedVal::FWD10;
        break;
    case SpeedVal::FWD10:
        new_speed = SpeedVal::FWD20;
        break;
    case SpeedVal::FWD20:
        new_speed = SpeedVal::FWD30;
        break;
    case SpeedVal::FWD30:
        new_speed = SpeedVal::FWD40;
        break;
    case SpeedVal::FWD40:
        new_speed = SpeedVal::FWD50;
        break;
    case SpeedVal::FWD50:
        new_speed = SpeedVal::FWD60;
        break;
    case SpeedVal::FWD60:
        new_speed = SpeedVal::FWD70;
        break;
    case SpeedVal::FWD70:
        new_speed = SpeedVal::FWD80;
        break;
    case SpeedVal::FWD80:
        new_speed = SpeedVal::FWD90;
        break;
    case SpeedVal::FWD90:
        new_speed = SpeedVal::FWD100;
        break;
    case SpeedVal::REV10:
        new_speed = SpeedVal::STOP;
        break;
    case SpeedVal::REV20:
        new_speed = SpeedVal::REV10;
        break;
    case SpeedVal::REV30:
        new_speed = SpeedVal::REV20;
        break;
    case SpeedVal::REV40:
        new_speed = SpeedVal::REV30;
        break;
    case SpeedVal::REV50:
        new_speed = SpeedVal::REV40;
        break;
    case SpeedVal::REV60:
        new_speed = SpeedVal::REV50;
        break;
    case SpeedVal::REV70:
        new_speed = SpeedVal::REV60;
        break;
    case SpeedVal::REV80:
        new_speed = SpeedVal::REV70;
        break;
    case SpeedVal::REV90:
        new_speed = SpeedVal::REV80;
        break;
    case SpeedVal::REV100:
        new_speed = SpeedVal::REV90;
        break;
    default:
        return;
    }

    set_water_cooling(new_speed);

    for (ServoController *controller : _servo_controllers)
    {
        controller->set_speed(new_speed);
    }
    _speed = new_speed;
}

void ShipControl::speed_down()
{
    SpeedVal new_speed;

    switch (_speed)
    {
    case SpeedVal::STOP:
        new_speed = SpeedVal::REV10;
        break;
    case SpeedVal::FWD10:
        new_speed = SpeedVal::STOP;
        break;
    case SpeedVal::FWD20:
        new_speed = SpeedVal::FWD10;
        break;
    case SpeedVal::FWD30:
        new_speed = SpeedVal::FWD20;
        break;
    case SpeedVal::FWD40:
        new_speed = SpeedVal::FWD30;
        break;
    case SpeedVal::FWD50:
        new_speed = SpeedVal::FWD40;
        break;
    case SpeedVal::FWD60:
        new_speed = SpeedVal::FWD50;
        break;
    case SpeedVal::FWD70:
        new_speed = SpeedVal::FWD60;
        break;
    case SpeedVal::FWD80:
        new_speed = SpeedVal::FWD70;
        break;
    case SpeedVal::FWD90:
        new_speed = SpeedVal::FWD80;
        break;
    case SpeedVal::FWD100:
        new_speed = SpeedVal::FWD90;
        break;
    case SpeedVal::REV10:
        new_speed = SpeedVal::REV20;
        break;
    case SpeedVal::REV20:
        new_speed = SpeedVal::REV30;
        break;
    case SpeedVal::REV30:
        new_speed = SpeedVal::REV40;
        break;
    case SpeedVal::REV40:
        new_speed = SpeedVal::REV50;
        break;
    case SpeedVal::REV50:
        new_speed = SpeedVal::REV60;
        break;
    case SpeedVal::REV60:
        new_speed = SpeedVal::REV70;
        break;
    case SpeedVal::REV70:
        new_speed = SpeedVal::REV80;
        break;
    case SpeedVal::REV80:
        new_speed = SpeedVal::REV90;
        break;
    case SpeedVal::REV90:
        new_speed = SpeedVal::REV100;
        break;
    default:
        return;
    }

    set_water_cooling(new_speed);

    for (ServoController *controller : _servo_controllers)
    {
        controller->set_speed(new_speed);
    }
    _speed = new_speed;
}


void ShipControl::set_speed(const std::string &speed_str)
{
    SpeedVal new_speed = ServoController::str_to_speed(speed_str);
    set_water_cooling(new_speed);
    for (ServoController *controller : _servo_controllers)
    {
        controller->set_speed(new_speed);
    }
    _speed = new_speed;
}

void ShipControl::set_steering(const std::string &steering_str)
{
    SteeringVal new_steering = ServoController::str_to_steering(steering_str);
    for (ServoController *controller : _servo_controllers)
    {
        controller->set_steering(new_steering);
    }
    _steering = new_steering;
}

void ShipControl::setup_signals()
{
    struct sigaction act;
    sigset_t sigset;

    // set of signals to be blocked during signal handling
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGTERM);

    act.sa_handler = signal_handler;
    act.sa_mask = sigset;
    act.sa_flags = 0;

    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGQUIT, &act, nullptr);
    sigaction(SIGTERM, &act, nullptr);

    // ignore SIGPIPE
    struct sigaction ignore_act;
    std::memset(static_cast<void *>(&ignore_act), 0, sizeof(struct sigaction));
    ignore_act.sa_handler = SIG_IGN;
    ignore_act.sa_flags = 0;

    sigaction(SIGPIPE, &ignore_act, nullptr);
}

void ShipControl::set_water_cooling(SpeedVal speed)
{
    if (_water_cooling_switch == nullptr)
    {
        return;
    }
    if (_water_cooling_switch->is_ok() == false)
    {
        return;
    }

    if (speed == SpeedVal::STOP)
    {
        _water_cooling_switch->off();
    }
    else
    {
        _water_cooling_switch->on();
    }
}

} // namespace shipcontrol
