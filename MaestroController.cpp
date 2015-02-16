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

const int MAESTRO_MAX_FWD = 2000 * 4;
const int MAESTRO_STOP = 1470 * 4;
const int MAESTRO_MAX_REV = 992 * 4;

const int MAESTRO_FWD_RANGE = MAESTRO_MAX_FWD - MAESTRO_STOP -
                              ((MAESTRO_MAX_FWD - MAESTRO_STOP) % 10);
const int MAESTRO_REV_RANGE = MAESTRO_STOP - MAESTRO_MAX_REV -
                              ((MAESTRO_STOP - MAESTRO_MAX_REV) % 10);

const int MAESTRO_STRAIGHT = 1670 * 4;
const int MAESTRO_LEFT_MAX = 992 * 4;
const int MAESTRO_RIGHT_MAX = 2336 * 4;

const int MAESTRO_RIGHT_RANGE = MAESTRO_RIGHT_MAX - MAESTRO_STRAIGHT -
                                ((MAESTRO_RIGHT_MAX - MAESTRO_STRAIGHT) % 10);
const int MAESTRO_LEFT_RANGE = MAESTRO_STRAIGHT - MAESTRO_LEFT_MAX -
                               ((MAESTRO_STRAIGHT - MAESTRO_LEFT_MAX) % 10);

MaestroController::MaestroController(MaestroConfig &config) :
    _config(config),
    _dev(nullptr),
    _fd(-1)
{
    _dev = _config.get_maestro_dev();
    _engines = _config.get_engine_channels();
    _steering = _config.get_steering_channels();

    _log = Log::getInstance();

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
}

SpeedVal MaestroController::get_speed()
{
    if (is_sane() == false)
    {
        return SpeedVal::STOP;
    }

    // assume that all engines have the same speed
    MaestroCmd cmd(_fd, MaestroCmdCode::GETPOS, _engines[0]);
    unsigned char *pval = cmd.send();
    int val = (*(pval + 1) << 8) | (*pval);
    _log->write(LogLevel::DEBUG, "MaestroController::get_speed(), value=%d\n", val);
    return int_to_speed(val);
}

void MaestroController::set_speed(SpeedVal speed)
{
    if (is_sane() == false)
    {
        return;
    }

    int val = speed_to_int(speed);
    _log->write(LogLevel::DEBUG, "MaestroController::set_speed(), value=%d\n", val);
    unsigned char val0 = val & 0x7F;
    unsigned char val1 = (val >> 7) & 0x7F;

    // send command for each engine
    for (int engine : _engines) {
        MaestroCmd cmd(_fd, MaestroCmdCode::SETTARGET, engine, val0, val1);
        cmd.send();
    }
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
    _log->write(LogLevel::DEBUG, "MaestroController::get_steering(), value=%d\n", val);
    return int_to_steering(val);
}

void MaestroController::set_steering(SteeringVal steering)
{
    if (is_sane() == false)
    {
        return;
    }

    int val = steering_to_int(steering);
    _log->write(LogLevel::DEBUG, "MaestroController::set_steering(), value=%d\n", val);
    unsigned char val0 = val & 0x7F;
    unsigned char val1 = (val >> 7) & 0x7F;

    // send command for each steering servo
    for (int servo : _steering) {
        MaestroCmd cmd(_fd, MaestroCmdCode::SETTARGET, servo, val0, val1);
        cmd.send();
    }
}

int MaestroController::speed_to_int(SpeedVal speed)
{
    switch (speed)
    {
    case SpeedVal::STOP:
        return MAESTRO_STOP;
    case SpeedVal::FWD10:
        return MAESTRO_STOP + MAESTRO_FWD_RANGE / 10;
    case SpeedVal::FWD20:
        return MAESTRO_STOP + (MAESTRO_FWD_RANGE / 10) * 2;
    case SpeedVal::FWD30:
        return MAESTRO_STOP + (MAESTRO_FWD_RANGE / 10) * 3;
    case SpeedVal::FWD40:
        return MAESTRO_STOP + (MAESTRO_FWD_RANGE / 10) * 4;
    case SpeedVal::FWD50:
        return MAESTRO_STOP + (MAESTRO_FWD_RANGE / 10) * 5;
    case SpeedVal::FWD60:
        return MAESTRO_STOP + (MAESTRO_FWD_RANGE / 10) * 6;
    case SpeedVal::FWD70:
        return MAESTRO_STOP + (MAESTRO_FWD_RANGE / 10) * 7;
    case SpeedVal::FWD80:
        return MAESTRO_STOP + (MAESTRO_FWD_RANGE / 10) * 8;
    case SpeedVal::FWD90:
        return MAESTRO_STOP + (MAESTRO_FWD_RANGE / 10) * 9;
    case SpeedVal::FWD100:
        return MAESTRO_MAX_FWD;
    case SpeedVal::REV10:
        return MAESTRO_STOP - MAESTRO_REV_RANGE / 10;
    case SpeedVal::REV20:
        return MAESTRO_STOP - (MAESTRO_REV_RANGE / 10) * 2;
    case SpeedVal::REV30:
        return MAESTRO_STOP - (MAESTRO_REV_RANGE / 10) * 3;
    case SpeedVal::REV40:
        return MAESTRO_STOP - (MAESTRO_REV_RANGE / 10) * 4;
    case SpeedVal::REV50:
        return MAESTRO_STOP - (MAESTRO_REV_RANGE / 10) * 5;
    case SpeedVal::REV60:
        return MAESTRO_STOP - (MAESTRO_REV_RANGE / 10) * 6;
    case SpeedVal::REV70:
        return MAESTRO_STOP - (MAESTRO_REV_RANGE / 10) * 7;
    case SpeedVal::REV80:
        return MAESTRO_STOP - (MAESTRO_REV_RANGE / 10) * 8;
    case SpeedVal::REV90:
        return MAESTRO_STOP - (MAESTRO_REV_RANGE / 10) * 9;
    case SpeedVal::REV100:
        return MAESTRO_MAX_REV;
    }
}

SpeedVal MaestroController::int_to_speed(int speed)
{
    int val = 0;
    int diff = speed - MAESTRO_STOP;

    if (diff == 0)
    {
        return SpeedVal::STOP;
    }

    if (diff > 0)
    {
        // forward
        val = (diff * 10) / MAESTRO_FWD_RANGE;
    }
    else
    {
        // reverse
        val = (diff * 10) / MAESTRO_REV_RANGE;
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
    }
}

int MaestroController::steering_to_int(SteeringVal steering)
{
    switch (steering)
    {
    case SteeringVal::STRAIGHT:
        return MAESTRO_STRAIGHT;
    case SteeringVal::RIGHT10:
        return MAESTRO_STRAIGHT + (MAESTRO_RIGHT_RANGE / 10);
    case SteeringVal::RIGHT20:
        return MAESTRO_STRAIGHT + (MAESTRO_RIGHT_RANGE / 10) * 2;
    case SteeringVal::RIGHT30:
        return MAESTRO_STRAIGHT + (MAESTRO_RIGHT_RANGE / 10) * 3;
    case SteeringVal::RIGHT40:
        return MAESTRO_STRAIGHT + (MAESTRO_RIGHT_RANGE / 10) * 4;
    case SteeringVal::RIGHT50:
        return MAESTRO_STRAIGHT + (MAESTRO_RIGHT_RANGE / 10) * 5;
    case SteeringVal::RIGHT60:
        return MAESTRO_STRAIGHT + (MAESTRO_RIGHT_RANGE / 10) * 6;
    case SteeringVal::RIGHT70:
        return MAESTRO_STRAIGHT + (MAESTRO_RIGHT_RANGE / 10) * 7;
    case SteeringVal::RIGHT80:
        return MAESTRO_STRAIGHT + (MAESTRO_RIGHT_RANGE / 10) * 8;
    case SteeringVal::RIGHT90:
        return MAESTRO_STRAIGHT + (MAESTRO_RIGHT_RANGE / 10) * 9;
    case SteeringVal::RIGHT100:
        return MAESTRO_RIGHT_MAX;
    case SteeringVal::LEFT10:
        return MAESTRO_STRAIGHT - (MAESTRO_LEFT_RANGE / 10);
    case SteeringVal::LEFT20:
        return MAESTRO_STRAIGHT - (MAESTRO_LEFT_RANGE / 10) * 2;
    case SteeringVal::LEFT30:
        return MAESTRO_STRAIGHT - (MAESTRO_LEFT_RANGE / 10) * 3;
    case SteeringVal::LEFT40:
        return MAESTRO_STRAIGHT - (MAESTRO_LEFT_RANGE / 10) * 4;
    case SteeringVal::LEFT50:
        return MAESTRO_STRAIGHT - (MAESTRO_LEFT_RANGE / 10) * 5;
    case SteeringVal::LEFT60:
        return MAESTRO_STRAIGHT - (MAESTRO_LEFT_RANGE / 10) * 6;
    case SteeringVal::LEFT70:
        return MAESTRO_STRAIGHT - (MAESTRO_LEFT_RANGE / 10) * 7;
    case SteeringVal::LEFT80:
        return MAESTRO_STRAIGHT - (MAESTRO_LEFT_RANGE / 10) * 8;
    case SteeringVal::LEFT90:
        return MAESTRO_STRAIGHT - (MAESTRO_LEFT_RANGE / 10) * 9;
    case SteeringVal::LEFT100:
        return MAESTRO_LEFT_MAX;
    }
}

SteeringVal MaestroController::int_to_steering(int steering)
{
    int val = 0;
    int diff = steering - MAESTRO_STRAIGHT;

    if (diff == 0)
    {
        return SteeringVal::STRAIGHT;
    }

    if (diff > 0)
    {
        // right
        val = (diff * 10) / MAESTRO_RIGHT_RANGE;
    }
    else
    {
        // left
        val = (diff * 10) / MAESTRO_LEFT_RANGE;
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
