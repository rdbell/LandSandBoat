#include "test_map_networking_socket_gate_2660.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingSocketGate2660SelfTests() -> bool
{
    const bool ok = mapnetworkinghelpers::ShouldOpenSocket(false) && !mapnetworkinghelpers::ShouldOpenSocket(true);
    if (!ok)
    {
        std::cerr << "map networking socket gate 2660 self-test failed\n";
    }
    return ok;
}
