/*
 * Copyright (C) 2016 - 2024 Mikhail Sapozhnikov
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
#include "MaestroConfig.hpp"

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

    ASSERT_EQ(true, config.is_ok());

    const sc::key_map *keymap = config.get_keymap();
    sc::InputEventType evtType = keymap->at(KEY_1);
    ASSERT_EQ(sc::InputEventType::SPEED_UP, evtType);
    evtType = keymap->at(KEY_2);
    ASSERT_EQ(sc::InputEventType::SPEED_DOWN, evtType);

    const sc::rel_map *relmap = config.get_relmap();
    sc::RelEvent rel1{REL_Y, true};
    evtType = relmap->at(rel1);
    ASSERT_EQ(sc::InputEventType::TURN_RIGHT, evtType);
    sc::RelEvent rel2{REL_X, false};
    evtType = relmap->at(rel2);
    ASSERT_EQ(sc::InputEventType::TURN_LEFT, evtType);

    const char *maestro_dev = config.get_maestro_dev();
    ASSERT_STREQ("/dev/ttyACM0", maestro_dev);

    sc::SteeringCalibration steering_calibration = config.get_steering_calibration();
    ASSERT_EQ(1500, steering_calibration.straight);
    ASSERT_EQ(80, steering_calibration.step);

    std::vector<sc::MaestroEngine> engines = config.get_engine_channels();
    ASSERT_EQ(2, engines.size());
    ASSERT_EQ(8, engines[0].channel);
    ASSERT_TRUE(engines[0].fwd);
    ASSERT_EQ(0, engines[0].dir_channel);
    ASSERT_EQ(100, engines[0].stop);
    ASSERT_EQ(240, engines[0].step);
    ASSERT_EQ(1, engines[1].channel);
    ASSERT_FALSE(engines[1].fwd);
    ASSERT_EQ(sc::MaestroEngine::NO_CHANNEL, engines[1].dir_channel);
    ASSERT_EQ(2000, engines[1].stop);
    ASSERT_EQ(300, engines[1].step);

    std::vector<int> steering = config.get_steering_channels();
    ASSERT_EQ(1, steering.size());
    ASSERT_EQ(4, steering[0]);

    int dir_high = config.get_direction_high();
    ASSERT_EQ(1900, dir_high);
    int dir_low = config.get_direction_low();
    ASSERT_EQ(800, dir_low);

    std::string unix_socket = config.get_unix_socket_name();
    ASSERT_EQ("/tmp/scsocket", unix_socket);

    std::vector<sc::GPIOEngineConfig> gpio_engine_configs = config.get_gpio_engine_configs();
    ASSERT_EQ(4, gpio_engine_configs.size());

    ASSERT_STREQ("/dev/gpiochip0", gpio_engine_configs[0].chip_path.c_str());
    ASSERT_EQ(3, gpio_engine_configs[0].engine_line);
    ASSERT_EQ(100, gpio_engine_configs[0].pwm_period);
    ASSERT_EQ(sc::GPIOReverseMode::SAME_LINE, gpio_engine_configs[0].reverse_mode);

    ASSERT_STREQ("/dev/gpiochip0", gpio_engine_configs[1].chip_path.c_str());
    ASSERT_EQ(1, gpio_engine_configs[1].engine_line);
    ASSERT_EQ(2, gpio_engine_configs[1].dir_line);
    ASSERT_EQ(133, gpio_engine_configs[1].pwm_period);
    ASSERT_EQ(sc::GPIOReverseMode::DEDICATED_LINE, gpio_engine_configs[1].reverse_mode);

    ASSERT_STREQ("/dev/gpiochip1", gpio_engine_configs[2].chip_path.c_str());
    ASSERT_EQ(6, gpio_engine_configs[2].engine_line);
    ASSERT_EQ(80, gpio_engine_configs[2].pwm_period);
    ASSERT_EQ(sc::GPIOReverseMode::NO_REVERSE, gpio_engine_configs[2].reverse_mode);

    ASSERT_STREQ("/dev/gpiochip1", gpio_engine_configs[3].chip_path.c_str());
    ASSERT_STREQ("/sys/class/pwm/pwmchip0", gpio_engine_configs[3].syspwm_path.c_str());
    ASSERT_EQ(0, gpio_engine_configs[3].syspwm_num);
    ASSERT_EQ(11, gpio_engine_configs[3].dir_line);
    ASSERT_EQ(10, gpio_engine_configs[3].pwm_period);
    ASSERT_EQ(5, gpio_engine_configs[3].min_duty_cycle);
    ASSERT_EQ(65, gpio_engine_configs[3].max_duty_cycle);
    ASSERT_EQ(sc::GPIOReverseMode::DEDICATED_LINE, gpio_engine_configs[3].reverse_mode);

    std::vector<sc::LogBackendType> log_backends = config.get_log_backends();
    ASSERT_EQ(2, log_backends.size());
    ASSERT_EQ(sc::LogBackendType::CONSOLE, log_backends[0]);
    ASSERT_EQ(sc::LogBackendType::SYSLOG, log_backends[1]);

    sc::LogLevel log_level = config.get_log_level();
    ASSERT_EQ(sc::LogLevel::NOTICE, log_level);
}

} // namespace config_test
