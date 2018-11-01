/*
 * Copyright (C) 2016 - 2018 Mikhail Sapozhnikov
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

#include "shipcontrol.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>

namespace shipcontrol
{

static void signal_handler(int sig);

static bool stop = false;

ShipControl::ShipControl() :
    _config(nullptr),
    _evdevReader(nullptr),
    _controller(nullptr),
    _speed(SpeedVal::STOP),
    _steering(SteeringVal::STRAIGHT)
{
    _log = Log::getInstance();
    _log->add_backend(&_syslog);
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
    if (_controller != nullptr)
    {
        delete _controller;
    }
    Log::release();
}

int ShipControl::run()
{
    int ret = init();

    if (ret != RETVAL_OK)
    {
        return ret;
    }

    _evdevReader->start();

    // event handling loop
    while (true)
    {
        if (stop)
        {
            break;
        }

        InputEvent evt = _inputQueue.pop_blocking();
        switch (evt)
        {
        case InputEvent::TURN_RIGHT:
            turn_right();
            break;
        case InputEvent::TURN_LEFT:
            turn_left();
            break;
        case InputEvent::SPEED_UP:
            speed_up();
            break;
        case InputEvent::SPEED_DOWN:
            speed_down();
            break;
        default:
            break;
        }
    }

    _evdevReader->stop();

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
    _controller = new MaestroController(*_config);
    _speed = _controller->get_speed();
    _steering = _controller->get_steering();

    return RETVAL_OK;
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
    SteeringVal cur_steering = _controller->get_steering();
    SteeringVal new_steering = SteeringVal::RIGHT50;

    switch (cur_steering)
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

    _controller->set_steering(new_steering);
    _steering = _controller->get_steering();
}

void ShipControl::turn_left()
{
    SteeringVal cur_steering = _controller->get_steering();
    SteeringVal new_steering;

    switch (cur_steering)
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

    _controller->set_steering(new_steering);
    _steering = _controller->get_steering();
}

void ShipControl::speed_up()
{
    SpeedVal cur_speed = _controller->get_speed();
    SpeedVal new_speed;

    switch (cur_speed)
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

    _controller->set_speed(new_speed);
    _speed = _controller->get_speed();
}

void ShipControl::speed_down()
{
    SpeedVal cur_speed = _controller->get_speed();
    SpeedVal new_speed;

    switch (cur_speed)
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

    _controller->set_speed(new_speed);
    _speed = _controller->get_speed();
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
}

static void signal_handler(int sig)
{
    stop = true;
}

} // namespace shipcontrol
