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
#include <linux/input.h>
#include "Config.hpp"

#ifndef TESTCONFIG_FILE
#error "TESTCONFIG_FILE is not defined"
#endif

namespace sc = shipcontrol;

namespace config_test
{

TEST(Config, ConfigTest)
{
    std::string testfile(TESTCONFIG_FILE);
    sc::Config config(testfile);

    const sc::key_map *keymap = config.get_keymap();
    sc::InputEvent evt = keymap->at(KEY_1);
    ASSERT_EQ(sc::InputEvent::SPEED_UP, evt);
    evt = keymap->at(KEY_2);
    ASSERT_EQ(sc::InputEvent::SPEED_DOWN, evt);

    const sc::rel_map *relmap = config.get_relmap();
    sc::RelEvent rel1{REL_Y, true};
    evt = relmap->at(rel1);
    ASSERT_EQ(sc::InputEvent::TURN_RIGHT, evt);
    sc::RelEvent rel2{REL_X, false};
    evt = relmap->at(rel2);
    ASSERT_EQ(sc::InputEvent::TURN_LEFT, evt);

    const char *maestro_dev = config.get_maestro_dev();
    ASSERT_STREQ("/dev/ttyACM3", maestro_dev);

    std::vector<int> engines = config.get_engine_channels();
    ASSERT_EQ(2, engines.size());
    ASSERT_EQ(1, engines[0]);
    ASSERT_EQ(8, engines[1]);

    std::vector<int> steering = config.get_steering_channels();
    ASSERT_EQ(2, steering.size());
    ASSERT_EQ(2, steering[0]);
    ASSERT_EQ(6, steering[1]);

    sc::LogLevel log_level = config.get_log_level();
    ASSERT_EQ(sc::LogLevel::NOTICE, log_level);
}

} // namespace config_test
