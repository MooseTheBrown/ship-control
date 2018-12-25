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

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "MaestroController.hpp"
#include "ConsoleLog.hpp"

namespace sc = shipcontrol;

namespace maestro_test
{

// test config provider for MaestroController
class TestMaestroConfig : public sc::MaestroConfig
{
public:
    virtual const char *get_maestro_dev() { return "/dev/ttyACM0"; }

    virtual std::vector<int> get_engine_channels()
    {
        std::vector<int> engines;
        engines.push_back(0);
        engines.push_back(5);
        return engines;
    }

    virtual std::vector<int> get_steering_channels()
    {
        std::vector<int> steering;
        steering.push_back(3);
        return steering;
    }

    virtual sc::MaestroCalibration get_maestro_calibration()
    {
        sc::MaestroCalibration calibration;
        calibration.max_fwd = 8000;
        calibration.stop = 5920;
        calibration.max_rev = 3968;
        calibration.straight = 6680;
        calibration.left_max = 3968;
        calibration.right_max = 9344;
        return calibration;
    }
};

// fixture
class MaestroTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        _log = sc::Log::getInstance();
        _log->add_backend(&_clog);
        _log->set_level(sc::LogLevel::DEBUG);
        _controller = new sc::MaestroController(_testConfig);
    }

    virtual void TearDown()
    {
        delete _controller;
        sc::Log::release();
    }

protected:
    sc::MaestroController *_controller;
    TestMaestroConfig _testConfig;
    sc::Log *_log;
    sc::ConsoleLog _clog;
};

TEST_F(MaestroTest, SpeedTest)
{
    std::chrono::milliseconds duration(1000);
    ASSERT_EQ(sc::SpeedVal::STOP, _controller->get_speed());

    // forward
    _controller->set_speed(sc::SpeedVal::FWD10);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::FWD10, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::FWD20);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::FWD20, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::FWD30);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::FWD30, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::FWD40);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::FWD40, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::FWD50);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::FWD50, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::FWD60);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::FWD60, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::FWD70);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::FWD70, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::FWD80);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::FWD80, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::FWD90);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::FWD90, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::FWD100);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::FWD100, _controller->get_speed());

    // stop
    _controller->set_speed(sc::SpeedVal::STOP);
    std::this_thread::sleep_for(duration);

    // reverse
    _controller->set_speed(sc::SpeedVal::REV10);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::REV10, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::REV20);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::REV20, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::REV30);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::REV30, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::REV40);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::REV40, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::REV50);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::REV50, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::REV60);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::REV60, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::REV70);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::REV70, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::REV80);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::REV80, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::REV90);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::REV90, _controller->get_speed());

    _controller->set_speed(sc::SpeedVal::REV100);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SpeedVal::REV100, _controller->get_speed());
}

TEST_F(MaestroTest, SteeringTest)
{
    std::chrono::milliseconds duration(1000);

    ASSERT_EQ(sc::SteeringVal::STRAIGHT, _controller->get_steering());

    // right
    _controller->set_steering(sc::SteeringVal::RIGHT10);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::RIGHT10, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::RIGHT20);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::RIGHT20, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::RIGHT30);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::RIGHT30, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::RIGHT40);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::RIGHT40, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::RIGHT50);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::RIGHT50, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::RIGHT60);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::RIGHT60, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::RIGHT70);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::RIGHT70, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::RIGHT80);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::RIGHT80, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::RIGHT90);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::RIGHT90, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::RIGHT100);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::RIGHT100, _controller->get_steering());

    // straight
    _controller->set_steering(sc::SteeringVal::STRAIGHT);
    std::this_thread::sleep_for(duration);

    // left
    _controller->set_steering(sc::SteeringVal::LEFT10);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::LEFT10, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::LEFT20);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::LEFT20, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::LEFT30);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::LEFT30, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::LEFT40);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::LEFT40, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::LEFT50);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::LEFT50, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::LEFT60);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::LEFT60, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::LEFT70);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::LEFT70, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::LEFT80);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::LEFT80, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::LEFT90);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::LEFT90, _controller->get_steering());

    _controller->set_steering(sc::SteeringVal::LEFT100);
    std::this_thread::sleep_for(duration);
    ASSERT_EQ(sc::SteeringVal::LEFT100, _controller->get_steering());
}

} // namespace maestro_test
