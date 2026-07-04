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

#include "test_ipc_channel.h"

#include "common/zmq/channel.h"

#include <concurrentqueue.h>

#include <iostream>
#include <string>

namespace
{

template <typename T>
auto expectEqual(const T& actual, const T& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC channel self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testTryReceive() -> bool
{
    moodycamel::ConcurrentQueue<std::string> incoming;
    moodycamel::ConcurrentQueue<std::string> outgoing;
    ipc::Channel<std::string>                channel(incoming, outgoing);

    bool        ok = true;
    std::string value;
    ok = expectEqual(channel.tryReceive(value), false, "empty receive") && ok;

    incoming.enqueue("alpha");
    incoming.enqueue("bravo");

    ok = expectEqual(channel.tryReceive(value), true, "first receive ok") && ok;
    ok = expectEqual(value, std::string("alpha"), "first receive value") && ok;
    ok = expectEqual(channel.tryReceive(value), true, "second receive ok") && ok;
    ok = expectEqual(value, std::string("bravo"), "second receive value") && ok;
    ok = expectEqual(channel.tryReceive(value), false, "drained receive") && ok;
    ok = expectEqual(outgoing.size_approx(), static_cast<std::size_t>(0), "outgoing unchanged by receive") && ok;

    return ok;
}

auto testSend() -> bool
{
    moodycamel::ConcurrentQueue<int> incoming;
    moodycamel::ConcurrentQueue<int> outgoing;
    ipc::Channel<int>                channel(incoming, outgoing);

    channel.send(10);
    channel.send(20);

    bool ok = true;
    int  value{};
    ok = expectEqual(incoming.size_approx(), static_cast<std::size_t>(0), "incoming unchanged by send") && ok;
    ok = expectEqual(outgoing.try_dequeue(value), true, "first outbound ok") && ok;
    ok = expectEqual(value, 10, "first outbound value") && ok;
    ok = expectEqual(outgoing.try_dequeue(value), true, "second outbound ok") && ok;
    ok = expectEqual(value, 20, "second outbound value") && ok;
    ok = expectEqual(outgoing.try_dequeue(value), false, "drained outbound") && ok;

    return ok;
}

} // namespace

auto runIPCChannelSelfTests() -> bool
{
    return testTryReceive() && testSend();
}
