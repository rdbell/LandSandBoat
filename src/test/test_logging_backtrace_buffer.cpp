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

#include "test_logging_backtrace_buffer.h"

#include "common/logging.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expectEqual(const std::vector<std::string>& actual, const std::vector<std::string>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "logging backtrace buffer self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto clearBacktrace() -> void
{
    logging::GetBacktrace();
}

auto testBacktraceDrainsOldestFirst() -> bool
{
    clearBacktrace();

    logging::AddBacktrace("alpha");
    logging::AddBacktrace("bravo");
    logging::AddBacktrace("charlie");

    bool ok = true;
    ok = expectEqual(logging::GetBacktrace(), std::vector<std::string>{ "alpha", "bravo", "charlie" }, "oldest first") && ok;
    ok = expectEqual(logging::GetBacktrace(), std::vector<std::string>{}, "drained") && ok;

    return ok;
}

auto testBacktraceRvalueOverload() -> bool
{
    clearBacktrace();

    logging::AddBacktrace(std::string("temporary"));

    return expectEqual(logging::GetBacktrace(), std::vector<std::string>{ "temporary" }, "rvalue overload");
}

auto testBacktraceOverwritesOldestWhenFull() -> bool
{
    clearBacktrace();

    constexpr auto capacity = 32;
    for (auto i = 0; i < capacity + 2; ++i)
    {
        logging::AddBacktrace("line-" + std::to_string(i));
    }

    auto expected = std::vector<std::string>{};
    for (auto i = 2; i < capacity + 2; ++i)
    {
        expected.emplace_back("line-" + std::to_string(i));
    }

    return expectEqual(logging::GetBacktrace(), expected, "overwrite oldest");
}

} // namespace

auto runLoggingBacktraceBufferSelfTests() -> bool
{
    bool ok = true;

    ok = testBacktraceDrainsOldestFirst() && ok;
    ok = testBacktraceRvalueOverload() && ok;
    ok = testBacktraceOverwritesOldestWhenFull() && ok;

    clearBacktrace();

    return ok;
}
