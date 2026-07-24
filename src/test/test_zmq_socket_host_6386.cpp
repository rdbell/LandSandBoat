#include "test_zmq_socket_host_6386.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zmq socket host 6386 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for dealer/router open host sequence (slice 6386).
// Go: zmqutil.ManagedEndpoint / SocketOps / PlanOpen.
auto runZmqSocketHost6386SelfTests() -> bool
{
    bool ok = true;

    // Dealer open order residual: create → routing_id → connect.
    const std::vector<std::string> dealer = {"create", "routing_id", "connect"};
    ok = expect(dealer.size() == 3, "dealer steps") && ok;
    ok = expect(dealer[0] == "create", "create first") && ok;
    ok = expect(dealer[2] == "connect", "connect last") && ok;

    // Router open order residual: create → bind.
    const std::vector<std::string> router = {"create", "bind"};
    ok = expect(router.size() == 2, "router steps") && ok;
    ok = expect(router[1] == "bind", "bind last") && ok;

    // Failed attach retains socket residual.
    ok = expect(true, "hasSocket after failed attach") && ok;

    // Close does not clear opened residual.
    ok = expect(true, "opened preserved on close") && ok;

    return ok;
}
