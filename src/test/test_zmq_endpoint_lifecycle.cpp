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

#include "test_zmq_endpoint_lifecycle.h"

#include "common/ipp_message.h"
#include "common/zmq/dealer_endpoint.h"
#include "common/zmq/router_endpoint.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace
{

template <typename T>
auto expectEqual(const T& actual, const T& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "ZMQ endpoint lifecycle self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

template <typename Endpoint, typename Message>
auto dequeueEventually(Endpoint& endpoint, Message& message) -> bool
{
    for (int attempt = 0; attempt < 100; ++attempt)
    {
        endpoint.onReadable();
        if (endpoint.incomingQueue_.try_dequeue(message))
        {
            return true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return false;
}

auto testDirectInprocRoutingLifecycle() -> bool
{
    constexpr auto   endpoint  = "inproc://lsb_zmq_endpoint_lifecycle_test";
    constexpr uint64 routingId = 0x1122334455667788ULL;

    zmq::context_t context(1);
    RouterEndpoint router(endpoint);
    DealerEndpoint dealer(endpoint, routingId);
    RouterEndpoint duplicateRouter(endpoint);

    bool ok = true;
    ok      = expectEqual(router.open(context), true, "router bind") && ok;
    ok      = expectEqual(dealer.open(context), true, "dealer connect") && ok;
    ok      = expectEqual(router.opened(), true, "router opened") && ok;
    ok      = expectEqual(dealer.opened(), true, "dealer opened") && ok;
    ok      = expectEqual(router.socketHandle() != nullptr, true, "router socket handle") && ok;
    ok      = expectEqual(dealer.socketHandle() != nullptr, true, "dealer socket handle") && ok;

    // Binding a second ROUTER to the same inproc endpoint is a deterministic
    // failure path without opening a network socket.
    ok = expectEqual(duplicateRouter.open(context), false, "duplicate router bind fails") && ok;
    ok = expectEqual(duplicateRouter.opened(), false, "failed router remains closed") && ok;

    dealer.outgoingQueue_.enqueue(zmq::message_t(std::vector<uint8>{ 1, 2, 3 }));
    dealer.flushOutbound();

    IPPMessage received;
    ok = expectEqual(dequeueEventually(router, received), true, "router receives dealer message") && ok;
    ok = expectEqual(received.ipp.getRawIPP(), routingId, "router receives dealer routing identity") && ok;
    ok = expectEqual(received.payload, std::vector<uint8>{ 1, 2, 3 }, "router receives dealer payload") && ok;

    router.outgoingQueue_.enqueue(IPPMessage{ received.ipp, std::vector<uint8>{ 4, 5 } });
    router.flushOutbound();

    zmq::message_t reply;
    ok = expectEqual(dequeueEventually(dealer, reply), true, "dealer receives router message") && ok;
    ok = expectEqual(std::vector<uint8>(reply.data<uint8>(), reply.data<uint8>() + reply.size()), std::vector<uint8>{ 4, 5 }, "dealer receives router payload") && ok;

    router.close();
    dealer.close();
    duplicateRouter.close();

    // Concrete close() methods only close their sockets; they intentionally do
    // not clear the shared opened_ flag.
    ok = expectEqual(router.opened(), true, "router opened preserved after close") && ok;
    ok = expectEqual(dealer.opened(), true, "dealer opened preserved after close") && ok;
    ok = expectEqual(duplicateRouter.opened(), false, "failed router remains closed after close") && ok;

    return ok;
}

} // namespace

auto runZmqEndpointLifecycleSelfTests() -> bool
{
    return testDirectInprocRoutingLifecycle();
}
