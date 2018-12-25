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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "EvdevConfig.hpp"
#include "MaestroConfig.hpp"
#include "IPCConfig.hpp"
#include "Log.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace shipcontrol
{

// json-based configuration provider
class Config : public EvdevConfig,
               public MaestroConfig,
               public IPCConfig
{
public:
    Config(const std::string &filename);
    virtual ~Config();

    // EvdevConfig
    virtual const key_map *get_keymap() { return &_keymap; }
    virtual const rel_map *get_relmap() { return &_relmap; }
    // MaestroConfig
    virtual const char *get_maestro_dev() { return _maestro_dev.c_str(); }
    virtual std::vector<int> get_engine_channels() { return _engines; }
    virtual std::vector<int> get_steering_channels() { return _steering; }
    virtual MaestroCalibration get_maestro_calibration() { return _maestro_calibration; }
    // IPCConfig
    virtual std::string get_unix_socket_name() { return _unix_socket; }
    // general configuration
    std::vector<LogBackendType> get_log_backends() { return _logBackends; }
    LogLevel get_log_level() { return _logLevel; }

    bool is_ok() { return _is_ok; }

protected:
    bool _is_ok;
    std::vector<int> _engines;
    std::vector<int> _steering;
    MaestroCalibration _maestro_calibration;
    std::string _maestro_dev;
    std::unordered_map<std::string, int> _keystring_map;
    std::unordered_map<std::string, RelEvent> _relstring_map;
    std::unordered_map<std::string, InputEvent> _evtstring_map;
    key_map _keymap;
    rel_map _relmap;
    std::string _unix_socket;
    std::vector<LogBackendType> _logBackends;
    LogLevel _logLevel;

    void parse(const std::string &filename);
};

} // namespace shipcontrol

#endif // CONFIG_HPP
