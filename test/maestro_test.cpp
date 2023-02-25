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

    virtual std::vector<sc::MaestroEngine> get_engine_channels()
    {
        std::vector<sc::MaestroEngine> engines;
        engines.push_back(sc::MaestroEngine{8, 0, true, 100, 240});
        engines.push_back(sc::MaestroEngine{1, sc::MaestroEngine::NO_CHANNEL, false, 1500, 80});
        return engines;
    }

    virtual std::vector<int> get_steering_channels()
    {
        std::vector<int> steering;
        steering.push_back(5);
        return steering;
    }

    virtual sc::SteeringCalibration get_steering_calibration()
    {
        sc::SteeringCalibration calibration;
        calibration.straight = 1504;
        calibration.step = 80;
        return calibration;
    }

    virtual int get_direction_high()
    {
        return sc::MaestroConfig::DEFAULT_DIR_HIGH;
    }

    virtual int get_direction_low()
    {
        return sc::MaestroConfig::DEFAULT_DIR_LOW;
    }
};

// fixture
class MaestroTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        _log = sc::Log::getInstance();
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
};

TEST_F(MaestroTest, SpeedTest)
{
    std::chrono::milliseconds duration(500);
    ASSERT_EQ(sc::SpeedVal::STOP, _controller->get_speed());

    // forward
    _controller->set_speed(sc::SpeedVal::FWD10);
    ASSERT_EQ(sc::SpeedVal::FWD10, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::FWD20);
    ASSERT_EQ(sc::SpeedVal::FWD20, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::FWD30);
    ASSERT_EQ(sc::SpeedVal::FWD30, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::FWD40);
    ASSERT_EQ(sc::SpeedVal::FWD40, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::FWD50);
    ASSERT_EQ(sc::SpeedVal::FWD50, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::FWD60);
    ASSERT_EQ(sc::SpeedVal::FWD60, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::FWD70);
    ASSERT_EQ(sc::SpeedVal::FWD70, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::FWD80);
    ASSERT_EQ(sc::SpeedVal::FWD80, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::FWD90);
    ASSERT_EQ(sc::SpeedVal::FWD90, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::FWD100);
    ASSERT_EQ(sc::SpeedVal::FWD100, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::FWD50);
    ASSERT_EQ(sc::SpeedVal::FWD50, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    // stop
    _controller->set_speed(sc::SpeedVal::STOP);
    std::this_thread::sleep_for(duration);

    // reverse
    _controller->set_speed(sc::SpeedVal::REV10);
    ASSERT_EQ(sc::SpeedVal::REV10, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::REV20);
    ASSERT_EQ(sc::SpeedVal::REV20, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::REV30);
    ASSERT_EQ(sc::SpeedVal::REV30, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::REV40);
    ASSERT_EQ(sc::SpeedVal::REV40, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::REV50);
    ASSERT_EQ(sc::SpeedVal::REV50, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::REV60);
    ASSERT_EQ(sc::SpeedVal::REV60, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::REV70);
    ASSERT_EQ(sc::SpeedVal::REV70, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::REV80);
    ASSERT_EQ(sc::SpeedVal::REV80, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::REV90);
    ASSERT_EQ(sc::SpeedVal::REV90, _controller->get_speed());
    std::this_thread::sleep_for(duration);

    _controller->set_speed(sc::SpeedVal::REV100);
    ASSERT_EQ(sc::SpeedVal::REV100, _controller->get_speed());
    std::this_thread::sleep_for(duration);
}

TEST_F(MaestroTest, SteeringTest)
{
    std::chrono::milliseconds duration(500);

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
