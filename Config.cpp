#include "Config.hpp"
#include "json.hpp"
#include <fstream>
#include <linux/input.h>

using json = nlohmann::json;

namespace shipcontrol
{

Config::Config(const std::string &filename) :
    _is_ok(true)
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

    _evtstring_map.insert(std::make_pair("TURN_RIGHT", InputEvent::TURN_RIGHT));
    _evtstring_map.insert(std::make_pair("TURN_LEFT", InputEvent::TURN_LEFT));
    _evtstring_map.insert(std::make_pair("SPEED_UP", InputEvent::SPEED_UP));
    _evtstring_map.insert(std::make_pair("SPEED_DOWN", InputEvent::SPEED_DOWN));

    // parse the config
    parse(filename);
}

Config::~Config()
{
}

void Config::parse(const std::string &filename)
{
    json j;
    std::ifstream in(filename.c_str());

    in >> j;

    // get engine channels
    if (j.find("engines") != j.end())
    {
        auto engines = j["engines"];
        for (auto engine : engines)
        {
            _engines.push_back(engine);
        }
    }
    else
    {
        _is_ok = false;
    }

    // get steering channels
    if (j.find("steering") != j.end())
    {
        auto steering = j["steering"];
        for (auto st : steering)
        {
            _steering.push_back(st);
        }
    }
    else
    {
        _is_ok = false;
    }

    // get maestro device
    if (j.find("maestro_device") != j.end())
    {
        auto maestro_dev = j["maestro_device"];
        _maestro_dev = maestro_dev.get<std::string>();
    }
    else
    {
        _is_ok = false;
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
    else
    {
        _is_ok = false;
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
    else
    {
        _is_ok = false;
    }
}

} // namespace shipcontrol
