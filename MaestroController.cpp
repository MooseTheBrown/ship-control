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

#include "MaestroController.hpp"
#include "MaestroCmd.hpp"

#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

namespace shipcontrol
{

MaestroController::MaestroController(MaestroConfig &config) :
    _config(config),
    _dev(nullptr),
    _fd(-1)
{
    _dev = _config.get_maestro_dev();
    _engines = _config.get_engine_channels();
    _steering = _config.get_steering_channels();
    _steering_calibration = _config.get_steering_calibration();
    _dir_high = _config.get_direction_high();
    _dir_low = _config.get_direction_low();

    _log = Log::getInstance();
    _log->write(LogLevel::DEBUG, "MaestroController ctor\n");

    // open and configure Maestro serial device
    if (_dev != nullptr) {
        _fd = open(_dev, O_RDWR, 0);

        if (_fd != -1)
        {
            struct termios options;
            tcgetattr(_fd, &options);
            options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
            options.c_oflag &= ~(ONLCR | OCRNL);
            tcsetattr(_fd, TCSANOW, &options);

            // set speed to STOP and steering position to STRAIGHT
            set_speed(SpeedVal::STOP);
            set_steering(SteeringVal::STRAIGHT);
        }
        else
        {
            _log->write(LogLevel::ERROR, "Couldn't open Maestro device, error code %d\n", errno);
        }
    }
}

MaestroController::~MaestroController()
{
    if (is_sane() == true)
    {
        set_speed(SpeedVal::STOP);
        set_steering(SteeringVal::STRAIGHT);
    }

    if (_fd != -1)
    {
        close(_fd);
    }

    Log::release();
}

SpeedVal MaestroController::get_speed()
{
    if (is_sane() == false)
    {
        return SpeedVal::STOP;
    }

    return _cur_speed;
}

void MaestroController::set_speed(SpeedVal speed)
{
    if (is_sane() == false)
    {
        return;
    }

    // send commands for each engine
    for (MaestroEngine engine : _engines)
    {
        int val = speed_to_int(speed, engine);

        _log->write(LogLevel::DEBUG,
                "MaestroController::set_speed(), channel=%d, fwd=%d, value=%d\n",
                engine.channel, engine.fwd, val);
        // Pololu protocol requires values in quarter-microseconds
        val *= 4;
        unsigned char val0 = val & 0x7F;
        unsigned char val1 = (val >> 7) & 0x7F;
        MaestroCmd cmd(_fd, MaestroCmdCode::SETTARGET, engine.channel, val0, val1);
        cmd.send();

        // set rotation direction using separate channel if needed
        if ((engine.dir_channel != MaestroEngine::NO_CHANNEL) && (speed != SpeedVal::STOP))
        {
            int dir_val = 0;
            if (static_cast<int>(speed) > static_cast<int>(SpeedVal::STOP))
            {
                dir_val = engine.fwd ? _dir_high : _dir_low;
            }
            else
            {
                dir_val = engine.fwd ? _dir_low : _dir_high;
            }
            _log->write(LogLevel::DEBUG,
                "MaestroController::set_speed(), direction channel=%d, dir_val=%d\n",
                engine.dir_channel, dir_val);
            dir_val *= 4;
            unsigned char dir_val0 = dir_val & 0x7F;
            unsigned char dir_val1 = (dir_val >> 7) & 0x7F;
            MaestroCmd cmd(_fd, MaestroCmdCode::SETTARGET, engine.dir_channel, dir_val0, dir_val1);
            cmd.send();
        }
    }

    _cur_speed = speed;
}

SteeringVal MaestroController::get_steering()
{
    if (is_sane() == false)
    {
        return SteeringVal::STRAIGHT;
    }

    return _cur_steering;
}

void MaestroController::set_steering(SteeringVal steering)
{
    if (is_sane() == false)
    {
        return;
    }

    int val = steering_to_int(steering);
    _log->write(LogLevel::DEBUG, "MaestroController::set_steering(), value=%d\n", val);
    // Pololu protocol requires values in quarter-microseconds
    val *= 4;
    unsigned char val0 = val & 0x7F;
    unsigned char val1 = (val >> 7) & 0x7F;

    // send command for each steering servo
    for (int servo : _steering)
    {
        MaestroCmd cmd(_fd, MaestroCmdCode::SETTARGET, servo, val0, val1);
        cmd.send();
    }

    _cur_steering = steering;
}

int MaestroController::speed_to_int(SpeedVal speed, const MaestroEngine &engine)
{
    if (speed == SpeedVal::STOP)
    {
        return engine.stop;
    }
    else
    {
        int multiplier = static_cast<int>(speed);
        if (engine.dir_channel != MaestroEngine::NO_CHANNEL)
        {
            // rotation direction is handled through a separate channel
            multiplier = std::abs(multiplier);
        }
        else if (!engine.fwd)
        {
            multiplier = multiplier * (-1);
        }

        return engine.stop + (multiplier * engine.step);
    }
}

int MaestroController::steering_to_int(SteeringVal steering)
{
    int multiplier = static_cast<int>(steering);
    return _steering_calibration.straight + (multiplier * _steering_calibration.step);
}

bool MaestroController::is_sane()
{
    if (_fd == -1)
    {
        _log->write(LogLevel::ERROR, "MaestroController isn't sane: no Maestro device\n");
        return false;
    }

    if (_engines.size() == 0)
    {
        _log->write(LogLevel::ERROR, "MaestroController isn't sane: no engines\n");
        return false;
    }

    if (_steering.size() == 0)
    {
        _log->write(LogLevel::ERROR, "MaestroController isn't sane: no steering\n");
        return false;
    }

    return true;
}

} // namespace shipcontrol
