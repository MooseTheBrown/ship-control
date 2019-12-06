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
#include <linux/uinput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include "EvdevReader.hpp"
#include "ConsoleLog.hpp"

namespace sc = shipcontrol;

namespace evdev_test
{

#define TEST_DEV_NAME "EvdevTestDevice"

// config provider for EvdevReader
class TestEvdevConfig : public sc::EvdevConfig
{
public:
    TestEvdevConfig();
    virtual const sc::key_map *get_keymap() { return &_keymap; }
    virtual const sc::rel_map *get_relmap() { return &_relmap; }
protected:
    sc::key_map _keymap;
    sc::rel_map _relmap;
};

TestEvdevConfig::TestEvdevConfig()
{
    _keymap.insert(std::make_pair(KEY_W, sc::InputEvent::SPEED_UP));
    _keymap.insert(std::make_pair(KEY_S, sc::InputEvent::SPEED_DOWN));

    _relmap.insert(std::make_pair(sc::RelEvent{REL_X, false}, sc::InputEvent::TURN_LEFT));
    _relmap.insert(std::make_pair(sc::RelEvent{REL_X, true}, sc::InputEvent::TURN_RIGHT));
}

// test fixture
class EvdevTest : public ::testing::Test
{
public:
    EvdevTest();
    virtual void SetUp();
    virtual void TearDown();
protected:
    sc::Log *_log;
    int _fd;
    std::string _device;
    sc::EvdevReader *_reader;
    TestEvdevConfig _config;
    sc::InputQueue _queue;

    void find_device();
};

EvdevTest::EvdevTest()
: _reader(nullptr)
{
}

void EvdevTest::SetUp()
{
    _log = sc::Log::getInstance();
    _log->set_level(sc::LogLevel::DEBUG);

    _fd = open("/dev/uinput", O_WRONLY);
    if (_fd < 0)
    {
        std::cout << "Failed to open /dev/uinput" << std::endl;
        ASSERT_TRUE(false);
        return;
    }

    ioctl(_fd, UI_SET_EVBIT, EV_REL);
    ioctl(_fd, UI_SET_RELBIT, REL_X);
    ioctl(_fd, UI_SET_RELBIT, REL_Y);
    ioctl(_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(_fd, UI_SET_KEYBIT, KEY_W);
    ioctl(_fd, UI_SET_KEYBIT, KEY_S);

    uinput_user_dev uidev;
    std::memset(&uidev, 0, sizeof (uidev));
    std::snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, TEST_DEV_NAME);
    write(_fd, &uidev, sizeof (uidev));
    ioctl(_fd, UI_DEV_CREATE);

    find_device();

    _reader = new sc::EvdevReader(_config, _device, _queue);
    _reader->start();
}

void EvdevTest::TearDown()
{
    if (_reader != nullptr)
    {
        _reader->stop();
        // let evdev reader finish its shutdown
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        delete _reader;
    }
    if (_fd > 0)
    {
        ioctl(_fd, UI_DEV_DESTROY);
        close(_fd);
    }
    sc::Log::release();
}

// find device, which has been created through uinput in SetUp()
void EvdevTest::find_device()
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
        if (std::strncmp(name, TEST_DEV_NAME, sizeof (name)) == 0)
        {
            _device = std::move(filename);
            close(fd);
            break;
        }
        close(fd);
    }

    closedir(dir);
}

TEST_F(EvdevTest, EventTest)
{
    input_event event;

    // report key, wait and verify that the input queue contains corresponding event
    event.type = EV_KEY;
    event.code = KEY_W;
    event.value = 1;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ASSERT_FALSE(_queue.is_empty());
    sc::InputEvent evt = _queue.pop();
    ASSERT_EQ(sc::InputEvent::SPEED_UP, evt);
    ASSERT_TRUE(_queue.is_empty());

    // report SPEED_DOWN
    event.type = EV_KEY;
    event.code = KEY_S;
    event.value = 1;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ASSERT_FALSE(_queue.is_empty());
    evt = _queue.pop();
    ASSERT_EQ(sc::InputEvent::SPEED_DOWN, evt);
    ASSERT_TRUE(_queue.is_empty());

    // report key up event and verify that it is ignored
    event.type = EV_KEY;
    event.code = KEY_S;
    event.value = 0;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ASSERT_TRUE(_queue.is_empty());

    // report left turn
    event.type = EV_REL;
    event.code = REL_X;
    event.value = -1;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ASSERT_FALSE(_queue.is_empty());
    evt = _queue.pop();
    ASSERT_EQ(sc::InputEvent::TURN_LEFT, evt);
    ASSERT_TRUE(_queue.is_empty());

    // report right turn
    event.type = EV_REL;
    event.code = REL_X;
    event.value = 1;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ASSERT_FALSE(_queue.is_empty());
    evt = _queue.pop();
    ASSERT_EQ(sc::InputEvent::TURN_RIGHT, evt);
    ASSERT_TRUE(_queue.is_empty());

    // report Y axis and verify that it is ignored
    event.type = EV_REL;
    event.code = REL_Y;
    event.value = 1;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(_fd, &event, sizeof (input_event));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ASSERT_TRUE(_queue.is_empty());
}

TEST_F(EvdevTest, BlockingTest)
{
    std::thread thread([this]()->void
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        input_event event;
        event.type = EV_KEY;
        event.code = KEY_W;
        event.value = 1;
        write(_fd, &event, sizeof (input_event));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        event.type = EV_SYN;
        event.code = SYN_REPORT;
        event.value = 0;
        write(_fd, &event, sizeof (input_event));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    sc::InputEvent evt = _queue.pop_blocking();
    ASSERT_EQ(sc::InputEvent::SPEED_UP, evt);

    thread.join();
}

} // namespace evdev_test
