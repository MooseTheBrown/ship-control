#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "MaestroController.hpp"

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
};

// fixture
class MaestroTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        _controller = new sc::MaestroController(_testConfig);
    }

    virtual void TearDown()
    {
        delete _controller;
    }

protected:
    sc::MaestroController *_controller;
    TestMaestroConfig _testConfig;
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

} // namespace maestro_test
