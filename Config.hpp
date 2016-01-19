#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "EvdevConfig.hpp"
#include "MaestroConfig.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace shipcontrol
{

// Configuration provider for EvdevReader
class Config : public EvdevConfig, MaestroConfig
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

    bool is_ok() { return _is_ok; }

protected:
    bool _is_ok;
    std::vector<int> _engines;
    std::vector<int> _steering;
    std::string _maestro_dev;
    std::unordered_map<std::string, int> _keystring_map;
    std::unordered_map<std::string, RelEvent> _relstring_map;
    std::unordered_map<std::string, InputEvent> _evtstring_map;
    key_map _keymap;
    rel_map _relmap;

    void parse(const std::string &filename);
};

} // namespace shipcontrol

#endif // CONFIG_HPP
