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

#include "test_zmq_endpoint_opened.h"

#include "common/zmq/endpoint.h"

#include <iostream>
#include <string>
#include <zmq.hpp>

namespace
{

template <typename T>
auto expectEqual(const T& actual, const T& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "ZMQ endpoint opened self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

class FakeEndpoint final : public ZmqEndpoint
{
public:
    auto open(zmq::context_t&) -> bool override
    {
        opened_.store(true, std::memory_order_release);
        return opened();
    }

    auto failOpen() -> bool
    {
        opened_.store(false, std::memory_order_release);
        return opened();
    }

    auto close() -> void override
    {
        opened_.store(false, std::memory_order_release);
    }

    auto socketHandle() -> void* override
    {
        return nullptr;
    }

    auto onReadable() -> void override
    {
    }

    auto flushOutbound() -> void override
    {
    }

    auto setOpened(bool opened) -> void
    {
        opened_.store(opened, std::memory_order_release);
    }
};

auto testOpenedState() -> bool
{
    FakeEndpoint  endpoint;
    zmq::context_t context;

    bool ok = true;
    ok      = expectEqual(endpoint.opened(), false, "initial opened") && ok;
    ok      = expectEqual(endpoint.open(context), true, "open result") && ok;
    ok      = expectEqual(endpoint.opened(), true, "opened after open") && ok;
    endpoint.close();
    ok = expectEqual(endpoint.opened(), false, "opened after close") && ok;
    endpoint.setOpened(true);
    ok = expectEqual(endpoint.opened(), true, "manual opened true") && ok;
    ok = expectEqual(endpoint.failOpen(), false, "failed open result") && ok;
    ok = expectEqual(endpoint.opened(), false, "opened after failed open") && ok;

    return ok;
}

} // namespace

auto runZmqEndpointOpenedSelfTests() -> bool
{
    return testOpenedState();
}
