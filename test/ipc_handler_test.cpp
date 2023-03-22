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
#include "IPCRequestHandler.hpp"
#include "ConsoleLog.hpp"
#include "json.hpp"

namespace sc = shipcontrol;
using json = nlohmann::json;

namespace ipc_handler_test
{

// mock data provider for the test
class TestDataProvider: public sc::DataProvider
{
public:
    virtual sc::SpeedVal get_speed();
    virtual sc::SteeringVal get_steering();
};

sc::SpeedVal TestDataProvider::get_speed()
{
    return sc::SpeedVal::FWD100;
}

sc::SteeringVal TestDataProvider::get_steering()
{
    return sc::SteeringVal::RIGHT50;
}

// test fixture
class IPCHandlerTest : public ::testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
protected:
    TestDataProvider _data_provider;
    sc::InputQueue _input_queue;
    sc::IPCRequestHandler *_handler;
    sc::Log *_log;
};

void IPCHandlerTest::SetUp()
{
    _log = sc::Log::getInstance();
    _log->set_level(sc::LogLevel::DEBUG);
    _handler = new sc::IPCRequestHandler(_input_queue, _data_provider);
}

void IPCHandlerTest::TearDown()
{
    if (_handler != nullptr)
    {
        delete _handler;
    }
    sc::Log::release();
}

TEST_F(IPCHandlerTest, ValidCommand)
{
    json rq;
    json resp;

    rq["type"] = "cmd";
    rq["cmd"] = "speed_up";
    resp = json::parse(_handler->handleRequest(rq.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "ok");
    ASSERT_FALSE(_input_queue.is_empty());
    ASSERT_EQ(sc::InputEventType::SPEED_UP, _input_queue.pop().type);
    ASSERT_TRUE(_input_queue.is_empty());

    rq["type"] = "cmd";
    rq["cmd"] = "speed_down";
    resp = json::parse(_handler->handleRequest(rq.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "ok");
    ASSERT_FALSE(_input_queue.is_empty());
    ASSERT_EQ(sc::InputEventType::SPEED_DOWN, _input_queue.pop().type);
    ASSERT_TRUE(_input_queue.is_empty());

    rq["type"] = "cmd";
    rq["cmd"] = "turn_left";
    resp = json::parse(_handler->handleRequest(rq.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "ok");
    ASSERT_FALSE(_input_queue.is_empty());
    ASSERT_EQ(sc::InputEventType::TURN_LEFT, _input_queue.pop().type);
    ASSERT_TRUE(_input_queue.is_empty());

    rq["type"] = "cmd";
    rq["cmd"] = "turn_right";
    resp = json::parse(_handler->handleRequest(rq.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "ok");
    ASSERT_FALSE(_input_queue.is_empty());
    ASSERT_EQ(sc::InputEventType::TURN_RIGHT, _input_queue.pop().type);
    ASSERT_TRUE(_input_queue.is_empty());

    rq["type"] = "cmd";
    rq["cmd"] = "set_speed";
    rq["data"] = "rev20";
    resp = json::parse(_handler->handleRequest(rq.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "ok");
    ASSERT_FALSE(_input_queue.is_empty());
    sc::InputEvent evt = _input_queue.pop();
    ASSERT_TRUE(_input_queue.is_empty());
    ASSERT_EQ(sc::InputEventType::SET_SPEED, evt.type);
    ASSERT_EQ("rev20", evt.data);

    rq["type"] = "cmd";
    rq["cmd"] = "set_steering";
    rq["data"] = "left70";
    resp = json::parse(_handler->handleRequest(rq.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "ok");
    ASSERT_FALSE(_input_queue.is_empty());
    evt = _input_queue.pop();
    ASSERT_TRUE(_input_queue.is_empty());
    ASSERT_EQ(sc::InputEventType::SET_STEERING, evt.type);
    ASSERT_EQ("left70", evt.data);
}

TEST_F(IPCHandlerTest, InvalidCommand)
{
    json rq;
    json resp;

    rq["typeooooooooooo"] = "cmd";
    rq["cmd"] = "speed_up";
    resp = json::parse(_handler->handleRequest(rq.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "fail");

    json rq1;
    rq1["type"] = "blahblah";
    rq1["cmd"] = "speed_up";
    resp = json::parse(_handler->handleRequest(rq1.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "fail");

    json rq2;
    rq2["type"] = "cmd";
    resp = json::parse(_handler->handleRequest(rq2.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "fail");

    json rq3;
    rq3["type"] = "cmd";
    rq3["cmd"] = "set_speed";
    resp = json::parse(_handler->handleRequest(rq3.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "fail");

    rq3["data"] = "somedata";
    resp = json::parse(_handler->handleRequest(rq3.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "fail");

    json rq4;
    rq4["type"] = "cmd";
    rq4["cmd"] = "set_steering";
    resp = json::parse(_handler->handleRequest(rq4.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "fail");

    rq4["data"] = "somedata";
    resp = json::parse(_handler->handleRequest(rq4.dump()));
    ASSERT_TRUE(resp["status"].get<std::string>() == "fail");
}

TEST_F(IPCHandlerTest, Query)
{
    json rq;
    json resp;

    rq["type"] = "query";
    resp = json::parse(_handler->handleRequest(rq.dump()));
    ASSERT_EQ("fwd100", resp["speed"]);
    ASSERT_EQ("right50", resp["steering"]);
}

} // namespace ipc_handler_test
