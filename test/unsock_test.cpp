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

#include <gtest/gtest.h>
#include <string>
#include <chrono>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "UnixListener.hpp"
#include "ConsoleLog.hpp"
#include "json.hpp"

namespace sc = shipcontrol;
using json = nlohmann::json;

namespace unsock_test
{

#define TESTSOCKET_NAME "testsocket"
#define READBUF_SIZE 4096

// client capable of sending messages and reading responses to/from Unix socket
class UnsockClient
{
public:
    UnsockClient();
    virtual ~UnsockClient();
    void connect();
    void disconnect();
    // send message via unix socket and get response
    std::string send_msg(const std::string &msg);
    bool is_connected() { return _connected; }
protected:
    int _socket;
    sc::Log *_log;
    bool _connected;
};

UnsockClient::UnsockClient()
: _connected(false),
  _socket(-1)
{
    _log = sc::Log::getInstance();
}

UnsockClient::~UnsockClient()
{
    if (_connected == true)
    {
        disconnect();
    }
    sc::Log::release();
}

void UnsockClient::connect()
{
    _socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_socket == -1)
    {
        _log->write(sc::LogLevel::ERROR, "UnsockClient failed to open client socket, err = %d\n", errno);
        return;
    }
    sockaddr_un addr;
    std::memset(&addr, 0, sizeof(sockaddr_un));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, TESTSOCKET_NAME, sizeof(addr.sun_path) - 1);
    if (::connect(_socket, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un)) == -1)
    {
        _log->write(sc::LogLevel::ERROR, "UnsockClient failed to connect to the server socket, err = %d\n", errno);
    }
    _connected = true;
}

void UnsockClient::disconnect()
{
    if (_socket != -1)
    {
        close(_socket);
    }
    _connected = false;
}

std::string UnsockClient::send_msg(const std::string &msg)
{
    ssize_t count = write(_socket, reinterpret_cast<const void *>(msg.c_str()), msg.length());
    if (count != msg.length())
    {
        _log->write(sc::LogLevel::ERROR, "UnsockTest::send_msg(), written %d bytes instead of %d\n", count, msg.length());
        return std::string("");
    }

    char *readbuf = new char[READBUF_SIZE];
    std::memset(readbuf, 0, READBUF_SIZE);
    count = read(_socket, reinterpret_cast<void *>(readbuf), READBUF_SIZE);
    if (count <= 0)
    {
        _log->write(sc::LogLevel::ERROR, "UnsockTest::send_msg(), failed to read from socket\n");
        return std::string("");
    }

    std::string ret(readbuf);
    delete readbuf;

    return ret;
}

// fixture
class UnsockTest : public ::testing::Test, sc::IPCConfig, sc::DataProvider
{
public:
    // ::testing:Test
    virtual void SetUp();
    virtual void TearDown();
    // shipcontrol::IPCConfig
    virtual std::string get_unix_socket_name() { return std::string(TESTSOCKET_NAME); }
    // shipcontrol::DataProvider
    virtual sc::SpeedVal get_speed() { return sc::SpeedVal::REV30; }
    virtual sc::SteeringVal get_steering() { return sc::SteeringVal::LEFT10; }
protected:
    sc::InputQueue _inputQueue;
    sc::IPCRequestHandler *_requestHandler;
    sc::UnixListener *_unixListener;
    sc::Log *_log;
    sc::ConsoleLog _clog;
};

void UnsockTest::SetUp()
{
    _log = sc::Log::getInstance();
    _log->add_backend(&_clog);
    _log->set_level(sc::LogLevel::DEBUG);

    _requestHandler = new sc::IPCRequestHandler(_inputQueue, *this);
    _unixListener = new sc::UnixListener(*this, *_requestHandler);
    _unixListener->start();
    // let unix listener create its socket and connect to it
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void UnsockTest::TearDown()
{
    _unixListener->stop();
    // let unix listener and all its threads finish shut-down
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    delete _unixListener;
    delete _requestHandler;
    sc::Log::release();
    unlink(TESTSOCKET_NAME);
}

TEST_F(UnsockTest, Query)
{
    UnsockClient client;
    json rq;
    json resp;
    std::string resp_str;

    client.connect();

    try
    {
        rq["type"] = "query";
        resp_str = client.send_msg(rq.dump());
        resp = json::parse(resp_str);
        ASSERT_EQ("rev30", resp["speed"]);
        ASSERT_EQ("left10", resp["steering"]);
    }
    catch (std::invalid_argument &e)
    {
        _log->write(sc::LogLevel::ERROR, "std::invalid_argument caught: %s\n", e.what());
        _log->write(sc::LogLevel::DEBUG, "response string: %s\n", resp_str.c_str());
        ASSERT_TRUE(false);
    }
}

TEST_F(UnsockTest, Command)
{
    UnsockClient client;
    json rq;
    json resp;
    std::string resp_str;

    client.connect();

    try
    {
        rq["type"] = "cmd";
        rq["cmd"] = "turn_left";
        resp_str = client.send_msg(rq.dump());
        resp = json::parse(resp_str);
        ASSERT_EQ("ok", resp["status"]);
        ASSERT_FALSE(_inputQueue.is_empty());
        ASSERT_EQ(sc::InputEvent::TURN_LEFT, _inputQueue.pop());
    }
    catch (std::invalid_argument &e)
    {
        _log->write(sc::LogLevel::ERROR, "std::invalid_argument caught: %s\n", e.what());
        _log->write(sc::LogLevel::DEBUG, "response string: %s\n", resp_str.c_str());
        ASSERT_TRUE(false);
    }
}

} // namespace unsock_test
