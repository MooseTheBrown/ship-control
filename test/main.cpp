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
#include <signal.h>
#include <cstring>
#include "ConsoleLog.hpp"

namespace sc = shipcontrol;

int main(int argc, char **argv)
{
    // ignore SIGPIPE
    struct sigaction ignore_act;
    std::memset(static_cast<void *>(&ignore_act), 0, sizeof(struct sigaction));
    ignore_act.sa_handler = SIG_IGN;
    ignore_act.sa_flags = 0;

    sigaction(SIGPIPE, &ignore_act, nullptr);

    sc::ConsoleLog clog;
    sc::Log *log = sc::Log::getInstance();
    log->add_backend(&clog);

    // run the tests
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    sc::Log::release();
    return ret;
}

void signal_handler(int sig) {}
