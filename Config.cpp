/*
 * Copyright (C) 2016 - 2025 Mikhail Sapozhnikov
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

#include "Config.hpp"
#include "json.hpp"
#include <fstream>
#include <cstring>
#include <linux/input.h>

using json = nlohmann::json;

namespace shipcontrol
{

Config::Config(const std::string &filename) :
    _is_ok(true),
    _logLevel(LogLevel::ERROR),
    _water_cooling_relay_config(nullptr)
{
    // prepare internal string-to-value maps for:
    // 1. Linux input keys
    // 2. REL_ events
    // 3. ship-control input events

    _keystring_map.insert(std::make_pair("KEY_ESC", KEY_ESC));
    _keystring_map.insert(std::make_pair("KEY_1", KEY_1));
    _keystring_map.insert(std::make_pair("KEY_2", KEY_2));
    _keystring_map.insert(std::make_pair("KEY_3", KEY_3));
    _keystring_map.insert(std::make_pair("KEY_4", KEY_4));
    _keystring_map.insert(std::make_pair("KEY_5", KEY_5));
    _keystring_map.insert(std::make_pair("KEY_6", KEY_6));
    _keystring_map.insert(std::make_pair("KEY_7", KEY_7));
    _keystring_map.insert(std::make_pair("KEY_8", KEY_8));
    _keystring_map.insert(std::make_pair("KEY_9", KEY_9));
    _keystring_map.insert(std::make_pair("KEY_0", KEY_0));
    _keystring_map.insert(std::make_pair("KEY_MINUS", KEY_MINUS));
    _keystring_map.insert(std::make_pair("KEY_EQUAL", KEY_EQUAL));
    _keystring_map.insert(std::make_pair("KEY_BACKSPACE", KEY_BACKSPACE));
    _keystring_map.insert(std::make_pair("KEY_TAB", KEY_TAB));
    _keystring_map.insert(std::make_pair("KEY_Q", KEY_Q));
    _keystring_map.insert(std::make_pair("KEY_W", KEY_W));
    _keystring_map.insert(std::make_pair("KEY_E", KEY_E));
    _keystring_map.insert(std::make_pair("KEY_R", KEY_R));
    _keystring_map.insert(std::make_pair("KEY_T", KEY_T));
    _keystring_map.insert(std::make_pair("KEY_Y", KEY_Y));
    _keystring_map.insert(std::make_pair("KEY_U", KEY_U));
    _keystring_map.insert(std::make_pair("KEY_I", KEY_I));
    _keystring_map.insert(std::make_pair("KEY_O", KEY_O));
    _keystring_map.insert(std::make_pair("KEY_P", KEY_P));
    _keystring_map.insert(std::make_pair("KEY_LEFTBRACE", KEY_LEFTBRACE));
    _keystring_map.insert(std::make_pair("KEY_RIGHTBRACE", KEY_RIGHTBRACE));
    _keystring_map.insert(std::make_pair("KEY_ENTER", KEY_ENTER));
    _keystring_map.insert(std::make_pair("KEY_LEFTCTRL", KEY_LEFTCTRL));
    _keystring_map.insert(std::make_pair("KEY_A", KEY_A));
    _keystring_map.insert(std::make_pair("KEY_S", KEY_S));
    _keystring_map.insert(std::make_pair("KEY_D", KEY_D));
    _keystring_map.insert(std::make_pair("KEY_F", KEY_F));
    _keystring_map.insert(std::make_pair("KEY_G", KEY_G));
    _keystring_map.insert(std::make_pair("KEY_H", KEY_H));
    _keystring_map.insert(std::make_pair("KEY_J", KEY_J));
    _keystring_map.insert(std::make_pair("KEY_K", KEY_K));
    _keystring_map.insert(std::make_pair("KEY_L", KEY_L));
    _keystring_map.insert(std::make_pair("KEY_SEMICOLON", KEY_SEMICOLON));
    _keystring_map.insert(std::make_pair("KEY_APOSTROPHE", KEY_APOSTROPHE));
    _keystring_map.insert(std::make_pair("KEY_GRAVE", KEY_GRAVE));
    _keystring_map.insert(std::make_pair("KEY_LEFTSHIFT", KEY_LEFTSHIFT));
    _keystring_map.insert(std::make_pair("KEY_BACKSLASH", KEY_BACKSLASH));
    _keystring_map.insert(std::make_pair("KEY_Z", KEY_Z));
    _keystring_map.insert(std::make_pair("KEY_X", KEY_X));
    _keystring_map.insert(std::make_pair("KEY_C", KEY_C));
    _keystring_map.insert(std::make_pair("KEY_V", KEY_V));
    _keystring_map.insert(std::make_pair("KEY_B", KEY_B));
    _keystring_map.insert(std::make_pair("KEY_N", KEY_N));
    _keystring_map.insert(std::make_pair("KEY_M", KEY_M));
    _keystring_map.insert(std::make_pair("KEY_COMMA", KEY_COMMA));
    _keystring_map.insert(std::make_pair("KEY_DOT", KEY_DOT));
    _keystring_map.insert(std::make_pair("KEY_SLASH", KEY_SLASH));
    _keystring_map.insert(std::make_pair("KEY_RIGHTSHIFT", KEY_RIGHTSHIFT));
    _keystring_map.insert(std::make_pair("KEY_KPASTERISK", KEY_KPASTERISK));
    _keystring_map.insert(std::make_pair("KEY_LEFTALT", KEY_LEFTALT));
    _keystring_map.insert(std::make_pair("KEY_SPACE", KEY_SPACE));
    _keystring_map.insert(std::make_pair("KEY_CAPSLOCK", KEY_CAPSLOCK));
    _keystring_map.insert(std::make_pair("KEY_F1", KEY_F1));
    _keystring_map.insert(std::make_pair("KEY_F2", KEY_F2));
    _keystring_map.insert(std::make_pair("KEY_F3", KEY_F3));
    _keystring_map.insert(std::make_pair("KEY_F4", KEY_F4));
    _keystring_map.insert(std::make_pair("KEY_F5", KEY_F5));
    _keystring_map.insert(std::make_pair("KEY_F6", KEY_F6));
    _keystring_map.insert(std::make_pair("KEY_F7", KEY_F7));
    _keystring_map.insert(std::make_pair("KEY_F8", KEY_F8));
    _keystring_map.insert(std::make_pair("KEY_F9", KEY_F9));
    _keystring_map.insert(std::make_pair("KEY_F10", KEY_F10));
    _keystring_map.insert(std::make_pair("KEY_NUMLOCK", KEY_NUMLOCK));
    _keystring_map.insert(std::make_pair("KEY_SCROLLLOCK", KEY_SCROLLLOCK));
    _keystring_map.insert(std::make_pair("KEY_KP7", KEY_KP7));
    _keystring_map.insert(std::make_pair("KEY_KP8", KEY_KP8));
    _keystring_map.insert(std::make_pair("KEY_KP9", KEY_KP9));
    _keystring_map.insert(std::make_pair("KEY_KPMINUS", KEY_KPMINUS));
    _keystring_map.insert(std::make_pair("KEY_KP4", KEY_KP4));
    _keystring_map.insert(std::make_pair("KEY_KP5", KEY_KP5));
    _keystring_map.insert(std::make_pair("KEY_KP6", KEY_KP6));
    _keystring_map.insert(std::make_pair("KEY_KPPLUS", KEY_KPPLUS));
    _keystring_map.insert(std::make_pair("KEY_KP1", KEY_KP1));
    _keystring_map.insert(std::make_pair("KEY_KP2", KEY_KP2));
    _keystring_map.insert(std::make_pair("KEY_KP3", KEY_KP3));
    _keystring_map.insert(std::make_pair("KEY_KP0", KEY_KP0));
    _keystring_map.insert(std::make_pair("KEY_KPDOT", KEY_KPDOT));
    _keystring_map.insert(std::make_pair("KEY_ZENKAKUHANKAKU", KEY_ZENKAKUHANKAKU));
    _keystring_map.insert(std::make_pair("KEY_102ND", KEY_102ND));
    _keystring_map.insert(std::make_pair("KEY_F11", KEY_F11));
    _keystring_map.insert(std::make_pair("KEY_F12", KEY_F12));
    _keystring_map.insert(std::make_pair("KEY_RO", KEY_RO));
    _keystring_map.insert(std::make_pair("KEY_KATAKANA", KEY_KATAKANA));
    _keystring_map.insert(std::make_pair("KEY_HIRAGANA", KEY_HIRAGANA));
    _keystring_map.insert(std::make_pair("KEY_HENKAN", KEY_HENKAN));
    _keystring_map.insert(std::make_pair("KEY_KATAKANAHIRAGANA", KEY_KATAKANAHIRAGANA));
    _keystring_map.insert(std::make_pair("KEY_MUHENKAN", KEY_MUHENKAN));
    _keystring_map.insert(std::make_pair("KEY_KPJPCOMMA", KEY_KPJPCOMMA));
    _keystring_map.insert(std::make_pair("KEY_KPENTER", KEY_KPENTER));
    _keystring_map.insert(std::make_pair("KEY_RIGHTCTRL", KEY_RIGHTCTRL));
    _keystring_map.insert(std::make_pair("KEY_KPSLASH", KEY_KPSLASH));
    _keystring_map.insert(std::make_pair("KEY_SYSRQ", KEY_SYSRQ));
    _keystring_map.insert(std::make_pair("KEY_RIGHTALT", KEY_RIGHTALT));
    _keystring_map.insert(std::make_pair("KEY_LINEFEED", KEY_LINEFEED));
    _keystring_map.insert(std::make_pair("KEY_HOME", KEY_HOME));
    _keystring_map.insert(std::make_pair("KEY_UP", KEY_UP));
    _keystring_map.insert(std::make_pair("KEY_PAGEUP", KEY_PAGEUP));
    _keystring_map.insert(std::make_pair("KEY_LEFT", KEY_LEFT));
    _keystring_map.insert(std::make_pair("KEY_RIGHT", KEY_RIGHT));
    _keystring_map.insert(std::make_pair("KEY_END", KEY_END));
    _keystring_map.insert(std::make_pair("KEY_DOWN", KEY_DOWN));
    _keystring_map.insert(std::make_pair("KEY_PAGEDOWN", KEY_PAGEDOWN));
    _keystring_map.insert(std::make_pair("KEY_INSERT", KEY_INSERT));
    _keystring_map.insert(std::make_pair("KEY_DELETE", KEY_DELETE));
    _keystring_map.insert(std::make_pair("KEY_MACRO", KEY_MACRO));
    _keystring_map.insert(std::make_pair("KEY_MUTE", KEY_MUTE));
    _keystring_map.insert(std::make_pair("KEY_VOLUMEDOWN", KEY_VOLUMEDOWN));
    _keystring_map.insert(std::make_pair("KEY_VOLUMEUP", KEY_VOLUMEUP));
    _keystring_map.insert(std::make_pair("KEY_POWER", KEY_POWER));
    _keystring_map.insert(std::make_pair("KEY_KPEQUAL", KEY_KPEQUAL));
    _keystring_map.insert(std::make_pair("KEY_KPPLUSMINUS", KEY_KPPLUSMINUS));
    _keystring_map.insert(std::make_pair("KEY_PAUSE", KEY_PAUSE));
    _keystring_map.insert(std::make_pair("KEY_SCALE", KEY_SCALE));
    _keystring_map.insert(std::make_pair("BTN_LEFT", BTN_LEFT));
    _keystring_map.insert(std::make_pair("BTN_RIGHT", BTN_RIGHT));


    _relstring_map.insert(std::make_pair("REL_X+", RelEvent{REL_X, true}));
    _relstring_map.insert(std::make_pair("REL_X-", RelEvent{REL_X, false}));
    _relstring_map.insert(std::make_pair("REL_Y+", RelEvent{REL_Y, true}));
    _relstring_map.insert(std::make_pair("REL_Y-", RelEvent{REL_Y, false}));

    _evtstring_map.insert(std::make_pair("TURN_RIGHT", InputEventType::TURN_RIGHT));
    _evtstring_map.insert(std::make_pair("TURN_LEFT", InputEventType::TURN_LEFT));
    _evtstring_map.insert(std::make_pair("SPEED_UP", InputEventType::SPEED_UP));
    _evtstring_map.insert(std::make_pair("SPEED_DOWN", InputEventType::SPEED_DOWN));

    std::memset(&_steering_calibration, 0, sizeof(SteeringCalibration));

    // parse the config
    parse(filename);
}

Config::~Config()
{
    delete _water_cooling_relay_config;
}

void Config::parse(const std::string &filename)
{
    json j;
    std::ifstream in(filename.c_str());

    if (!in.is_open())
    {
        _is_ok = false;
        return;
    }

    in >> j;

    // get engine channels
    if (j.find("maestro_engines") != j.end())
    {
        auto engines = j["maestro_engines"];
        for (auto engine : engines)
        {
            MaestroEngine me{0, true};
            if (engine.find("channel") != engine.end())
            {
                me.channel = engine["channel"].get<int>();
            }
            else
            {
                _is_ok = false;
            }
            if (engine.find("fwd") != engine.end())
            {
                me.fwd = engine["fwd"].get<bool>();
            }
            if (engine.find("dir_channel") != engine.end())
            {
                me.dir_channel = engine["dir_channel"].get<int>();
            }
            else
            {
                me.dir_channel = MaestroEngine::NO_CHANNEL;
            }
            if (engine.find("stop") != engine.end())
            {
                me.stop = engine["stop"].get<int>();
            }
            else
            {
                _is_ok = false;
            }
            if (engine.find("step") != engine.end())
            {
                me.step = engine["step"].get<int>();
            }
            else
            {
                _is_ok = false;
            }
            _engines.push_back(me);
        }
    }

    // get steering channels
    if (j.find("maestro_steering") != j.end())
    {
        auto steering = j["maestro_steering"];
        for (auto st : steering)
        {
            _steering.push_back(st);
        }
    }

    // get maestro device
    if (j.find("maestro_device") != j.end())
    {
        auto maestro_dev = j["maestro_device"];
        _maestro_dev = maestro_dev.get<std::string>();
    }

    // get maestro calibration data
    if (j.find("maestro_steering_calibration") != j.end())
    {
        auto calibration = j["maestro_steering_calibration"];
        if (calibration.find("straight") != calibration.end())
        {
            _steering_calibration.straight = calibration["straight"].get<int>();
        }
        else
        {
            _is_ok = false;
        }
        if (calibration.find("step") != calibration.end())
        {
            _steering_calibration.step = calibration["step"].get<int>();
        }
        else
        {
            _is_ok = false;
        }
    }

    // direction channels config
    if (j.find("maestro_direction") != j.end())
    {
        auto direction = j["maestro_direction"];
        if (direction.find("high") != direction.end())
        {
            _dir_high = direction["high"].get<int>();
        }
        else
        {
            _dir_high = MaestroConfig::DEFAULT_DIR_HIGH;
        }
        if (direction.find("low") != direction.end())
        {
            _dir_low = direction["low"].get<int>();
        }
        else
        {
            _dir_low = MaestroConfig::DEFAULT_DIR_LOW;
        }
    }
    else
    {
        _dir_high = MaestroConfig::DEFAULT_DIR_HIGH;
        _dir_low = MaestroConfig::DEFAULT_DIR_LOW;
    }

    // get keymap
    if (j.find("keymap") != j.end())
    {
        auto keymap = j["keymap"];
        for (json::iterator it = keymap.begin(); it != keymap.end(); it++)
        {
            std::string key = it.key();
            std::string val = it.value().get<std::string>();
            auto keymatch = _keystring_map.find(key);
            if (keymatch != _keystring_map.end())
            {
                auto valmatch = _evtstring_map.find(val);
                if (valmatch != _evtstring_map.end())
                {
                    _keymap.insert(std::make_pair(keymatch->second, valmatch->second));
                }
                else
                {
                    _is_ok = false;
                }
            }
            else
            {
                _is_ok = false;
            }
        }
    }

    // get relmap
    if (j.find("relmap") != j.end())
    {
        auto relmap = j["relmap"];
        for (json::iterator it = relmap.begin(); it != relmap.end(); it++)
        {
            std::string key = it.key();
            std::string val = it.value().get<std::string>();
            auto keymatch = _relstring_map.find(key);
            if (keymatch != _relstring_map.end())
            {
                auto valmatch = _evtstring_map.find(val);
                if (valmatch != _evtstring_map.end())
                {
                    _relmap.insert(std::make_pair(keymatch->second, valmatch->second));
                }
                else
                {
                    _is_ok = false;
                }
            }
            else
            {
                _is_ok = false;
            }
        }
    }

    // get unix socket name
    if (j.find("unix_socket") != j.end())
    {
        _unix_socket = j["unix_socket"].get<std::string>();
    }
    else
    {
        _is_ok = false;
    }

    // get log backends
    if (j.find("logbackends") != j.end())
    {
        auto backends = j["logbackends"];
        for (auto backend : backends)
        {
            std::string str_backend = backend.get<std::string>();
            if (str_backend == "console")
            {
                _logBackends.push_back(LogBackendType::CONSOLE);
            }
            else if (str_backend == "syslog")
            {
                _logBackends.push_back(LogBackendType::SYSLOG);
            }
        }
    }
    else
    {
        _is_ok = false;
    }

    // get loglevel
    if (j.find("loglevel") != j.end())
    {
        auto log_level = j["loglevel"];
        std::string str_level = log_level.get<std::string>();
        if (str_level == "error") { _logLevel = LogLevel::ERROR; }
        else if (str_level == "notice") { _logLevel = LogLevel::NOTICE; }
        else if (str_level == "debug") { _logLevel = LogLevel::DEBUG; }
    }
    else
    {
        _logLevel = LogLevel::NOTICE;
    }

    // get GPIO engine settings
    if (j.find("gpio_engines") != j.end())
    {
        auto gpio_engines = j["gpio_engines"];
        for (auto gpio_engine : gpio_engines)
        {
            GPIOEngineConfig gpio_engine_config;

            if (gpio_engine.find("chip_path") != gpio_engine.end())
            {
                gpio_engine_config.chip_path = gpio_engine["chip_path"].get<std::string>();
            }
            if (gpio_engine.find("syspwm_path") != gpio_engine.end())
            {
                gpio_engine_config.syspwm_path = gpio_engine["syspwm_path"].get<std::string>();
            }
            if (gpio_engine.find("syspwm_num") != gpio_engine.end())
            {
                gpio_engine_config.syspwm_num = gpio_engine["syspwm_num"].get<int>();
            }
            if (gpio_engine.find("engine_line") != gpio_engine.end())
            {
                gpio_engine_config.engine_line = gpio_engine["engine_line"].get<int>();
            }
            if (gpio_engine.find("dir_line") != gpio_engine.end())
            {
                gpio_engine_config.dir_line = gpio_engine["dir_line"].get<int>();
            }
            if (gpio_engine.find("pwm_period") != gpio_engine.end())
            {
                gpio_engine_config.pwm_period = gpio_engine["pwm_period"].get<int>();
            }
            else
            {
                _is_ok = false;
            }
            if (gpio_engine.find("min_duty_cycle") != gpio_engine.end())
            {
                gpio_engine_config.min_duty_cycle = gpio_engine["min_duty_cycle"].get<int>();
            }
            if (gpio_engine.find("max_duty_cycle") != gpio_engine.end())
            {
                gpio_engine_config.max_duty_cycle = gpio_engine["max_duty_cycle"].get<int>();
            }
            if (gpio_engine.find("rev_mode") != gpio_engine.end())
            {
                std::string reverse_mode = gpio_engine["rev_mode"].get<std::string>();
                if (reverse_mode == "same_line")
                {
                    gpio_engine_config.reverse_mode = GPIOReverseMode::SAME_LINE;
                }
                else if (reverse_mode == "dedicated_line")
                {
                    gpio_engine_config.reverse_mode = GPIOReverseMode::DEDICATED_LINE;
                }
                else if (reverse_mode == "no_reverse")
                {
                    gpio_engine_config.reverse_mode = GPIOReverseMode::NO_REVERSE;
                }
                else
                {
                    _is_ok = false;
                }
            }
            else
            {
                _is_ok = false;
            }
            _gpio_engine_configs.push_back(gpio_engine_config);
        }
    }

    if (j.find("gpio_steering") != j.end())
    {
        auto gpio_steering_array = j["gpio_steering"];
        for (auto gpio_steering : gpio_steering_array)
        {
            GPIOSteeringConfig gpio_steering_config;

            if (gpio_steering.find("chip_path") != gpio_steering.end())
            {
                gpio_steering_config.chip_path = gpio_steering["chip_path"].get<std::string>();
            }
            if (gpio_steering.find("steering_line") != gpio_steering.end())
            {
                gpio_steering_config.steering_line = gpio_steering["steering_line"].get<int>();
            }
            if (gpio_steering.find("syspwm_path") != gpio_steering.end())
            {
                gpio_steering_config.syspwm_path = gpio_steering["syspwm_path"].get<std::string>();
            }
            if (gpio_steering.find("syspwm_num") != gpio_steering.end())
            {
                gpio_steering_config.syspwm_num = gpio_steering["syspwm_num"].get<int>();
            }
            if (gpio_steering.find("pwm_period") != gpio_steering.end())
            {
                gpio_steering_config.pwm_period = gpio_steering["pwm_period"].get<int>();
            }
            if (gpio_steering.find("min_duty_cycle") != gpio_steering.end())
            {
                gpio_steering_config.min_duty_cycle = gpio_steering["min_duty_cycle"].get<int>();
            }
            if (gpio_steering.find("max_duty_cycle") != gpio_steering.end())
            {
                gpio_steering_config.max_duty_cycle = gpio_steering["max_duty_cycle"].get<int>();
            }

            _gpio_steering_configs.push_back(gpio_steering_config);
        }
    }

    if (j.find("water_cooling_relay") != j.end())
    {
        _water_cooling_relay_config = new GPIOSwitchConfig();
        auto wc_relay = j["water_cooling_relay"];
        if (wc_relay.find("chip_path") != wc_relay.end())
        {
            _water_cooling_relay_config->chip_path = wc_relay["chip_path"].get<std::string>();
        }
        if (wc_relay.find("line") != wc_relay.end())
        {
            _water_cooling_relay_config->line_num = wc_relay["line"].get<unsigned int>();
        }
    }
}

} // namespace shipcontrol
