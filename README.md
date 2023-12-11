# Overview
ship-control is a group of S/W projects enabling operation and remote control of custom-built aquatic vessel based on RaspberryPi.

Current repository contains the ship-control process running on the target board, which is responsible for controlling ship's engine and steering.

Other related repositories:

1. [Linux system](https://github.com/MooseTheBrown/ship-buildroot) - Buildroot Linux system running on the ship's main board
2. [MQTT bridge](https://github.com/MooseTheBrown/ship-net-bridge) - network bridge enabling MQTT communication for the ship
3. [Android app](https://github.com/MooseTheBrown/ship-control-android) - Android app gathering ship data and controlling speed and steering

# Architecture

ship-control architecture is described in the [architecture document](./doc/Arch.md)

# Building & installing

ship-control is built using cmake (any version starting from 3.0 is supported).

## Dependencies

ship-control requires the following libraries:

1. libgpiod
2. boost_program_options
3. gtest (only for building and running unit tests)

## Compiling
Run the following commands from directory containing ship-control sources:

    mkdir build
    cd build
    cmake ../
    make

This will produce ship-control executable.

### Running unit tests
To build unit tests run `cmake -DBUILD_TESTS=ON` and then `make`

This will produce ship-control-test executable, which needs to be run to execute the tests.

## Installing
Run `make install` to install ship-control.

The easiest way to install ship-control to a RaspberryPi board is to use [customized Buildroot](https://github.com/MooseTheBrown/ship-buildroot), which builds and includes ship-control and ship-net-bridge into the target image.

# Confguring
Upon starting ship-control reads configuration file /etc/shipcontrol.conf. The file has JSON format.

Sample configuration is provided in [default config](./shipcontrol.conf)

## Configuration parameters
| Parameter | Type | Required | Description |
| --------- | ---- | -------- | ----------- |
| maestro_engines | array | No | Array of objects containing configuration of engines controlled by Pololu Maestro controller |
| maestro_engine.channel | integer | Yes | Maestro channel number for the engine |
| maestro_engine.fwd | boolean | No | Engine rotation direction flag. True - forward, false - reverse |
| maestro_engine.dir_channel | integer | No | Maestro direction channel number |
| maestro_engine.stop | integer | Yes | Stop value for the engine |
| maestro_engine.step | integer | Yes | Step value for the engine |
| maestro_steering | array | No | Array of integers specifying Maestro steering channel numbers |
| maestro_device | string | No | Path to Maestro character device, e.g. "/dev/ttyACM0" |
| maestro_steering_calibration | object | No | Pololu Maestro steering channels calibration |
| maestro_steering_calibration.straight | integer | Yes | Straight steering position |
| maestro_steering_calibration.step | integer | Yes | Step of steering adjustment |
| maestro_direction | object | No | Direction channels calibration |
| maestro_direction.high | integer | No | High value for direction channels |
| maestro_direction.low | integer | No | Low value for direction channels |
| gpio_engines | array | No | Array of GPIO engine configuration objects |
| gpio_engine.chip_path | string | No | Path to GPIO character device, e.g. "/dev/gpiochip0" |
| gpio_engine.engine_line | integer | No | Engine GPIO line number |
| gpio_engine.dir_line | integer | No | Engine rotation direction GPIO line number |
| gpio_engine.pwm_period | integer | Yes | GPIO PWM period in microseconds |
| gpio_engine.rev_mode | string | No | Reverse mode for the engine. Possible values: "same_line", "dedicated_line", "no_reverse" | 
| keymap | object | No | Mapping of keyboard events (as reported by evdev) to ship-control actions |
| relmap | object | No | Mapping of mouse movement events to ship-control actions |
| unix_socket | string | Yes | Path to unix socket, which ship-control listens to for remote commands |
| logbackends | array | Yes | Array of strings indicating which log backends ship-control should use. Supported backends: "syslog", "console" |
| loglevel | string | No | Log level. Possible values: "error", "notice" (default), "debug". |
