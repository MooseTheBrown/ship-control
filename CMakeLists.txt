#
# Copyright (C) 2016 - 2018 Mikhail Sapozhnikov
#
# This file is part of ship-control.
#
# ship-control is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ship-control is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ship-control.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required (VERSION 2.6)

project (ship-control)

option (BUILD_TESTS "Build tests" OFF)

set (COMMON_CXX_FLAGS "-std=c++11 -pthread")
set (TEST_CXX_FLAGS "")
if (BUILD_TESTS)
    set (TEST_CXX_FLAGS -DTESTCONFIG_FILE=\\\"${ship-control_SOURCE_DIR}/test/testconfig\\\")
endif (BUILD_TESTS)
set (CMAKE_BUILD_TYPE RelWithDebInfo)
set (CMAKE_LINKER_FLAGS -ldl)
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${COMMON_CXX_FLAGS} ${TEST_CXX_FLAGS} -ggdb")
set (CMAKE_CXX_FLAGS_RELEASE "${COMMON_CXX_FLAGS} ${TEST_CXX_FLAGS}")

include_directories (${ship-control_SOURCE_DIR})

set (SHIPCONTROL_SRC Log.cpp
                     ConsoleLog.cpp
                     SysLog.cpp
                     MaestroCmd.cpp
                     MaestroController.cpp
                     InputQueue.cpp
                     EvdevReader.cpp
                     EvdevConfig.cpp
                     Config.cpp
                     IPCRequestHandler.cpp
                     SingleThread.cpp
                     UnixListener.cpp
                     IPCClient.cpp
                     shipcontrol.cpp)
add_executable (ship-control ${SHIPCONTROL_SRC} main.cpp)

install (TARGETS ship-control DESTINATION bin)
install (FILES shipcontrol.conf DESTINATION /etc)

if (BUILD_TESTS)
    set (TESTS_SRC ${SHIPCONTROL_SRC}
                   test/main.cpp
                   test/maestro_test.cpp
                   test/evdev_test.cpp
                   test/ipc_handler_test.cpp
                   test/unsock_test.cpp
                   test/config_test.cpp)
    find_library (GTEST_LIB NAMES gtest)
    if (${GTEST_LIB} EQUAL "GTEST_LIB-NOTFOUND")
        message(FATAL_ERROR "Google Test not found")
    endif (${GTEST_LIB} EQUAL "GTEST_LIB-NOTFOUND")
    add_executable (ship-control-test ${TESTS_SRC})
    target_link_libraries (ship-control-test ${GTEST_LIB})
endif (BUILD_TESTS)
