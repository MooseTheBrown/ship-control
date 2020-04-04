/*
 * Copyright (C) 2016 Mikhail Sapozhnikov
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
    _calibration = _config.get_maestro_calibration();
    _fwd_range = _calibration.max_fwd - _calibration.stop -
                 ((_calibration.max_fwd - _calibration.stop) % 10);
    _rev_range = _calibration.stop - _calibration.max_rev -
                 ((_calibration.stop - _calibration.max_rev) % 10);
    _right_range = _calibration.right_max - _calibration.straight -
                   ((_calibration.right_max - _calibration.straight) % 10);
    _left_range = _calibration.straight - _calibration.left_max -
                  ((_calibration.straight - _calibration.left_max) % 10);

    _log = Log::getInstance();
    _log->write(LogLevel::DEBUG, "MaestroController ctor\n");
    _log->write(LogLevel::DEBUG, "_fwd_range = %d, _rev_range = %d, _right_range = %d, _left_range = %d\n",
            _fwd_range, _rev_range, _right_range, _left_range);

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

    // assume that all engines have the same speed
    MaestroCmd cmd(_fd, MaestroCmdCode::GETPOS, _engines[0].channel);
    unsigned char *pval = cmd.send();
    int val = (*(pval + 1) << 8) | (*pval);
    // Pololu protocol works with quarter-microseconds
    val /= 4;
    _log->write(LogLevel::DEBUG, "MaestroController::get_speed(), value=%d\n", val);

    SpeedVal maestro_speed;
    if (_engines[0].fwd)
    {
        maestro_speed = int_to_speed(val);
    }
    else
    {
        maestro_speed = mirror_speed(int_to_speed(val));
    }

    if (_cur_speed != maestro_speed)
    {
        // Sometimes Maestro reports really strange speed values, which can not possibly
        // be correct
        _log->write(LogLevel::NOTICE, "Maestro reported incorrect speed: %d, expecting %d\n",
                maestro_speed, _cur_speed);
    }

    return _cur_speed;
}

void MaestroController::set_speed(SpeedVal speed)
{
    if (is_sane() == false)
    {
        return;
    }

    // send command for each engine
    for (MaestroEngine engine : _engines)
    {
        int val = 0;
        if (engine.fwd)
        {
            val = speed_to_int(speed);
        }
        else
        {
            val = speed_to_int(mirror_speed(speed));
        }

        _log->write(LogLevel::DEBUG,
                "MaestroController::set_speed(), channel=%d, fwd=%d, value=%d\n",
                engine.channel, engine.fwd, val);
        // Pololu protocol requires values in quarter-microseconds
        val *= 4;
        unsigned char val0 = val & 0x7F;
        unsigned char val1 = (val >> 7) & 0x7F;
        MaestroCmd cmd(_fd, MaestroCmdCode::SETTARGET, engine.channel, val0, val1);
        cmd.send();
    }

    _cur_speed = speed;
}

SteeringVal MaestroController::get_steering()
{
    if (is_sane() == false)
    {
        return SteeringVal::STRAIGHT;
    }

    // assume that all steering servos have the same position
    MaestroCmd cmd(_fd, MaestroCmdCode::GETPOS, _steering[0]);
    unsigned char *pval = cmd.send();
    int val = (*(pval + 1) << 8) | (*pval);
    // Pololu protocol works with quarter-microseconds
    val /= 4;
    _log->write(LogLevel::DEBUG, "MaestroController::get_steering(), value=%d\n", val);

    SteeringVal maestro_steering = int_to_steering(val);
    if (_cur_steering != maestro_steering)
    {
        // Sometimes Maestro reports really strange values, which can not possibly
        // be correct.
        _log->write(LogLevel::NOTICE, "Maestro reported incorrect steering: %d, expecting %d\n",
                maestro_steering, _cur_steering);
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

int MaestroController::speed_to_int(SpeedVal speed)
{
    switch (speed)
    {
    case SpeedVal::STOP:
        return _calibration.stop;
    case SpeedVal::FWD10:
        return _calibration.stop + _fwd_range / 10;
    case SpeedVal::FWD20:
        return _calibration.stop + (_fwd_range / 10) * 2;
    case SpeedVal::FWD30:
        return _calibration.stop + (_fwd_range / 10) * 3;
    case SpeedVal::FWD40:
        return _calibration.stop + (_fwd_range / 10) * 4;
    case SpeedVal::FWD50:
        return _calibration.stop + (_fwd_range / 10) * 5;
    case SpeedVal::FWD60:
        return _calibration.stop + (_fwd_range / 10) * 6;
    case SpeedVal::FWD70:
        return _calibration.stop + (_fwd_range / 10) * 7;
    case SpeedVal::FWD80:
        return _calibration.stop + (_fwd_range / 10) * 8;
    case SpeedVal::FWD90:
        return _calibration.stop + (_fwd_range / 10) * 9;
    case SpeedVal::FWD100:
        return _calibration.max_fwd;
    case SpeedVal::REV10:
        return _calibration.stop - _rev_range / 10;
    case SpeedVal::REV20:
        return _calibration.stop - (_rev_range / 10) * 2;
    case SpeedVal::REV30:
        return _calibration.stop - (_rev_range / 10) * 3;
    case SpeedVal::REV40:
        return _calibration.stop - (_rev_range / 10) * 4;
    case SpeedVal::REV50:
        return _calibration.stop - (_rev_range / 10) * 5;
    case SpeedVal::REV60:
        return _calibration.stop - (_rev_range / 10) * 6;
    case SpeedVal::REV70:
        return _calibration.stop - (_rev_range / 10) * 7;
    case SpeedVal::REV80:
        return _calibration.stop - (_rev_range / 10) * 8;
    case SpeedVal::REV90:
        return _calibration.stop - (_rev_range / 10) * 9;
    case SpeedVal::REV100:
        return _calibration.max_rev;
    }
}

SpeedVal MaestroController::int_to_speed(int speed)
{
    int val = 0;
    int diff = speed - _calibration.stop;

    if (diff == 0)
    {
        return SpeedVal::STOP;
    }

    if (diff > 0)
    {
        // forward
        val = (diff * 10) / _fwd_range;
    }
    else
    {
        // reverse
        val = (diff * 10) / _rev_range;
    }

    switch (val)
    {
    case 1:
        return SpeedVal::FWD10;
    case 2:
        return SpeedVal::FWD20;
    case 3:
        return SpeedVal::FWD30;
    case 4:
        return SpeedVal::FWD40;
    case 5:
        return SpeedVal::FWD50;
    case 6:
        return SpeedVal::FWD60;
    case 7:
        return SpeedVal::FWD70;
    case 8:
        return SpeedVal::FWD80;
    case 9:
        return SpeedVal::FWD90;
    case 10:
        return SpeedVal::FWD100;
    case -1:
        return SpeedVal::REV10;
    case -2:
        return SpeedVal::REV20;
    case -3:
        return SpeedVal::REV30;
    case -4:
        return SpeedVal::REV40;
    case -5:
        return SpeedVal::REV50;
    case -6:
        return SpeedVal::REV60;
    case -7:
        return SpeedVal::REV70;
    case -8:
        return SpeedVal::REV80;
    case -9:
        return SpeedVal::REV90;
    case -10:
        return SpeedVal::REV100;
    default:
        return SpeedVal::STOP;
    }
}

SpeedVal MaestroController::mirror_speed(SpeedVal speed)
{
    switch (speed)
    {
    case SpeedVal::STOP:
        return SpeedVal::STOP;
    case SpeedVal::FWD10:
        return SpeedVal::REV10;
    case SpeedVal::FWD20:
        return SpeedVal::REV20;
    case SpeedVal::FWD30:
        return SpeedVal::REV30;
    case SpeedVal::FWD40:
        return SpeedVal::REV40;
    case SpeedVal::FWD50:
        return SpeedVal::REV50;
    case SpeedVal::FWD60:
        return SpeedVal::REV60;
    case SpeedVal::FWD70:
        return SpeedVal::REV70;
    case SpeedVal::FWD80:
        return SpeedVal::REV80;
    case SpeedVal::FWD90:
        return SpeedVal::REV90;
    case SpeedVal::FWD100:
        return SpeedVal::REV100;
    case SpeedVal::REV10:
        return SpeedVal::FWD10;
    case SpeedVal::REV20:
        return SpeedVal::FWD20;
    case SpeedVal::REV30:
        return SpeedVal::FWD30;
    case SpeedVal::REV40:
        return SpeedVal::FWD40;
    case SpeedVal::REV50:
        return SpeedVal::FWD50;
    case SpeedVal::REV60:
        return SpeedVal::FWD60;
    case SpeedVal::REV70:
        return SpeedVal::FWD70;
    case SpeedVal::REV80:
        return SpeedVal::FWD80;
    case SpeedVal::REV90:
        return SpeedVal::FWD90;
    case SpeedVal::REV100:
        return SpeedVal::FWD100;
    }
}

int MaestroController::steering_to_int(SteeringVal steering)
{
    switch (steering)
    {
    case SteeringVal::STRAIGHT:
        return _calibration.straight;
    case SteeringVal::RIGHT10:
        return _calibration.straight + (_right_range / 10);
    case SteeringVal::RIGHT20:
        return _calibration.straight + (_right_range / 10) * 2;
    case SteeringVal::RIGHT30:
        return _calibration.straight + (_right_range / 10) * 3;
    case SteeringVal::RIGHT40:
        return _calibration.straight + (_right_range / 10) * 4;
    case SteeringVal::RIGHT50:
        return _calibration.straight + (_right_range / 10) * 5;
    case SteeringVal::RIGHT60:
        return _calibration.straight + (_right_range / 10) * 6;
    case SteeringVal::RIGHT70:
        return _calibration.straight + (_right_range / 10) * 7;
    case SteeringVal::RIGHT80:
        return _calibration.straight + (_right_range / 10) * 8;
    case SteeringVal::RIGHT90:
        return _calibration.straight + (_right_range / 10) * 9;
    case SteeringVal::RIGHT100:
        return _calibration.right_max;
    case SteeringVal::LEFT10:
        return _calibration.straight - (_left_range / 10);
    case SteeringVal::LEFT20:
        return _calibration.straight - (_left_range / 10) * 2;
    case SteeringVal::LEFT30:
        return _calibration.straight - (_left_range / 10) * 3;
    case SteeringVal::LEFT40:
        return _calibration.straight - (_left_range / 10) * 4;
    case SteeringVal::LEFT50:
        return _calibration.straight - (_left_range / 10) * 5;
    case SteeringVal::LEFT60:
        return _calibration.straight - (_left_range / 10) * 6;
    case SteeringVal::LEFT70:
        return _calibration.straight - (_left_range / 10) * 7;
    case SteeringVal::LEFT80:
        return _calibration.straight - (_left_range / 10) * 8;
    case SteeringVal::LEFT90:
        return _calibration.straight - (_left_range / 10) * 9;
    case SteeringVal::LEFT100:
        return _calibration.left_max;
    }
}

SteeringVal MaestroController::int_to_steering(int steering)
{
    int val = 0;
    int diff = steering - _calibration.straight;

    if (diff == 0)
    {
        return SteeringVal::STRAIGHT;
    }

    if (diff > 0)
    {
        // right
        val = (diff * 10) / _right_range;
    }
    else
    {
        // left
        val = (diff * 10) / _left_range;
    }

    switch (val)
    {
    case 1:
        return SteeringVal::RIGHT10;
    case 2:
        return SteeringVal::RIGHT20;
    case 3:
        return SteeringVal::RIGHT30;
    case 4:
        return SteeringVal::RIGHT40;
    case 5:
        return SteeringVal::RIGHT50;
    case 6:
        return SteeringVal::RIGHT60;
    case 7:
        return SteeringVal::RIGHT70;
    case 8:
        return SteeringVal::RIGHT80;
    case 9:
        return SteeringVal::RIGHT90;
    case 10:
        return SteeringVal::RIGHT100;
    case -1:
        return SteeringVal::LEFT10;
    case -2:
        return SteeringVal::LEFT20;
    case -3:
        return SteeringVal::LEFT30;
    case -4:
        return SteeringVal::LEFT40;
    case -5:
        return SteeringVal::LEFT50;
    case -6:
        return SteeringVal::LEFT60;
    case -7:
        return SteeringVal::LEFT70;
    case -8:
        return SteeringVal::LEFT80;
    case -9:
        return SteeringVal::LEFT90;
    case -10:
        return SteeringVal::LEFT100;
    default:
        return SteeringVal::STRAIGHT;
    }
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
