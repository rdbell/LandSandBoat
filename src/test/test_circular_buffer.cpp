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

#include "test_circular_buffer.h"

#include "common/circular_buffer.h"

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "circular buffer self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

template <typename Exception, typename Func>
auto expectThrows(Func&& fn, const std::string& label) -> bool
{
    try
    {
        std::forward<Func>(fn)();
    }
    catch (const Exception&)
    {
        return true;
    }
    catch (...)
    {
        std::cerr << "circular buffer self-test failed: " << label << " wrong exception type\n";
        return false;
    }

    std::cerr << "circular buffer self-test failed: " << label << " did not throw\n";
    return false;
}

auto testInvalidAndEmpty() -> bool
{
    bool ok = true;

    ok = expectThrows<std::invalid_argument>([] { static_cast<void>(CircularBuffer<int>(0)); }, "zero capacity") && ok;
    ok = expectThrows<std::invalid_argument>([] { static_cast<void>(CircularBuffer<int>(3)); }, "non-power-of-two capacity") && ok;

    CircularBuffer<int> buffer(4);
    ok = expectEqual(buffer.size(), std::size_t{ 0 }, "empty size") && ok;
    ok = expectEqual(buffer.is_empty(), true, "empty is_empty") && ok;
    ok = expectEqual(buffer.is_full(), false, "empty is_full") && ok;
    ok = expectThrows<std::runtime_error>([&] { static_cast<void>(buffer.front()); }, "empty front") && ok;
    ok = expectThrows<std::runtime_error>([&] { static_cast<void>(buffer.dequeue()); }, "empty dequeue") && ok;

    return ok;
}

auto testFifoAndWrap() -> bool
{
    bool ok = true;

    CircularBuffer<int> buffer(4);
    buffer.enqueue(1);
    buffer.enqueue(2);
    buffer.enqueue(3);

    ok = expectEqual(buffer.size(), std::size_t{ 3 }, "three item size") && ok;
    ok = expectEqual(buffer.front(), 1, "front before wrap") && ok;
    ok = expectEqual(buffer.dequeue(), 1, "first dequeue") && ok;

    buffer.enqueue(4);
    buffer.enqueue(5);
    ok = expectEqual(buffer.is_full(), true, "wrapped is_full") && ok;

    constexpr std::array<int, 4> expected = { 2, 3, 4, 5 };
    for (const auto value : expected)
    {
        ok = expectEqual(buffer.dequeue(), value, "wrapped fifo dequeue") && ok;
    }
    ok = expectEqual(buffer.is_empty(), true, "empty after drain") && ok;

    return ok;
}

auto testOverwriteWhenFull() -> bool
{
    bool ok = true;

    CircularBuffer<int> buffer(4);
    for (const auto value : { 1, 2, 3, 4, 5, 6 })
    {
        buffer.enqueue(value);
    }

    ok = expectEqual(buffer.size(), std::size_t{ 4 }, "overwrite size") && ok;
    ok = expectEqual(buffer.front(), 3, "overwrite front") && ok;

    constexpr std::array<int, 4> expected = { 3, 4, 5, 6 };
    for (const auto value : expected)
    {
        ok = expectEqual(buffer.dequeue(), value, "overwrite dequeue") && ok;
    }

    return ok;
}

auto testReuseAfterDrain() -> bool
{
    bool ok = true;

    CircularBuffer<std::string> buffer(2);
    buffer.enqueue("a");
    buffer.enqueue("b");
    ok = expectEqual(buffer.dequeue(), std::string("a"), "reuse first dequeue") && ok;
    ok = expectEqual(buffer.dequeue(), std::string("b"), "reuse second dequeue") && ok;

    buffer.enqueue("c");
    ok = expectEqual(buffer.front(), std::string("c"), "reuse front") && ok;
    ok = expectEqual(buffer.dequeue(), std::string("c"), "reuse dequeue") && ok;
    ok = expectEqual(buffer.is_empty(), true, "reuse empty") && ok;

    return ok;
}

} // namespace

auto runCircularBufferSelfTests() -> bool
{
    bool ok = true;

    ok = testInvalidAndEmpty() && ok;
    ok = testFifoAndWrap() && ok;
    ok = testOverwriteWhenFull() && ok;
    ok = testReuseAfterDrain() && ok;

    return ok;
}
