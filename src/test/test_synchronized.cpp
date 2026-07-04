/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_synchronized.h"

#include "common/synchronized.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <thread>

namespace
{
using namespace std::chrono_literals;

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "synchronized self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto waitFor(const std::atomic_bool& flag, const std::string& label) -> bool
{
    for (int i = 0; i < 100; ++i)
    {
        if (flag.load())
        {
            return true;
        }
        std::this_thread::sleep_for(10ms);
    }

    std::cerr << "synchronized self-test failed: " << label << '\n';
    return false;
}

auto expectStillFalse(const std::atomic_bool& flag, const std::string& label) -> bool
{
    std::this_thread::sleep_for(25ms);
    if (flag.load())
    {
        std::cerr << "synchronized self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testReadWriteValues() -> bool
{
    bool ok = true;

    Synchronized<int> value(10);
    ok = expectEqual(value.read([](const int& v) { return v; }), 10, "initial read") && ok;

    const auto written = value.write([](int& v) {
        v += 5;
        return v;
    });
    ok = expectEqual(written, 15, "write return") && ok;

    value.write([](int& v) {
        v *= 2;
        return 0;
    });
    ok = expectEqual(value.read([](const int& v) { return v; }), 30, "read after write") && ok;

    Synchronized<std::map<std::string, int>> table(std::map<std::string, int>{ { "alpha", 1 } });
    table.write([](auto& values) {
        values["beta"] = 2;
        return 0;
    });
    ok = expectEqual(table.read([](const auto& values) { return values.at("beta"); }), 2, "map write") && ok;

    return ok;
}

auto testExclusiveReadBlocksWrite() -> bool
{
    bool ok = true;

    Synchronized<int> value(1);
    std::atomic_bool  entered{ false };
    std::atomic_bool  release{ false };
    std::atomic_bool  writeReady{ false };
    std::atomic_bool  writeDone{ false };

    std::thread reader([&] {
        value.read([&](const int&) {
            entered = true;
            while (!release.load())
            {
                std::this_thread::sleep_for(1ms);
            }
            return 0;
        });
    });
    ok = waitFor(entered, "exclusive reader entered") && ok;

    std::thread writer([&] {
        writeReady = true;
        value.write([](int& v) {
            v = 2;
            return 0;
        });
        writeDone = true;
    });
    ok = waitFor(writeReady, "exclusive writer ready") && ok;
    ok = expectStillFalse(writeDone, "write completed while exclusive read held") && ok;

    release = true;
    reader.join();
    writer.join();

    ok = expectEqual(writeDone.load(), true, "exclusive writer completed") && ok;
    ok = expectEqual(value.read([](const int& v) { return v; }), 2, "exclusive final value") && ok;

    return ok;
}

auto testSharedReadsAndExclusiveWrite() -> bool
{
    bool ok = true;

    SynchronizedShared<int> value(10);
    std::atomic_bool        firstEntered{ false };
    std::atomic_bool        secondEntered{ false };
    std::atomic_bool        releaseReaders{ false };
    std::atomic_bool        writeReady{ false };
    std::atomic_bool        writeDone{ false };

    std::thread first([&] {
        value.read([&](const int&) {
            firstEntered = true;
            while (!releaseReaders.load())
            {
                std::this_thread::sleep_for(1ms);
            }
            return 0;
        });
    });
    ok = waitFor(firstEntered, "first shared reader entered") && ok;

    std::thread second([&] {
        value.read([&](const int&) {
            secondEntered = true;
            while (!releaseReaders.load())
            {
                std::this_thread::sleep_for(1ms);
            }
            return 0;
        });
    });
    ok = waitFor(secondEntered, "second shared reader entered") && ok;

    std::thread writer([&] {
        writeReady = true;
        value.write([](int& v) {
            v = 20;
            return 0;
        });
        writeDone = true;
    });
    ok = waitFor(writeReady, "shared writer ready") && ok;
    ok = expectStillFalse(writeDone, "shared write completed while readers held") && ok;

    releaseReaders = true;
    first.join();
    second.join();
    writer.join();

    ok = expectEqual(writeDone.load(), true, "shared writer completed") && ok;
    ok = expectEqual(value.read([](const int& v) { return v; }), 20, "shared final value") && ok;

    return ok;
}

} // namespace

auto runSynchronizedSelfTests() -> bool
{
    bool ok = true;

    ok = testReadWriteValues() && ok;
    ok = testExclusiveReadBlocksWrite() && ok;
    ok = testSharedReadsAndExclusiveWrite() && ok;

    return ok;
}
