#include "test_char_event_packet_1459.h"

#include "map/char_event_packet_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto RunPacketCase(bool stringsEmpty, bool hasParams, bool hasTextTable) -> std::vector<int>
{
    std::vector<int> calls{};
    chareventpackethelpers::SendAndFinalize(
        [&]() { calls.push_back(1); return stringsEmpty; },
        [&]() { calls.push_back(2); return hasParams; },
        [&]() { calls.push_back(3); return hasTextTable; },
        [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); },
        [&]() { calls.push_back(6); },
        [&]() { calls.push_back(7); },
        [&]() { calls.push_back(8); },
        [&]() { calls.push_back(9); });
    return calls;
}
} // namespace

auto runCharEventPacket1459SelfTests() -> bool
{
    bool ok = RunPacketCase(false, true, true) == std::vector<int>{ 1, 6, 7, 8, 9 };
    ok = RunPacketCase(true, true, true) == std::vector<int>{ 1, 2, 5, 7, 8, 9 } && ok;
    ok = RunPacketCase(true, false, true) == std::vector<int>{ 1, 2, 3, 5, 7, 8, 9 } && ok;
    ok = RunPacketCase(true, false, false) == std::vector<int>{ 1, 2, 3, 4, 7, 8, 9 } && ok;

    if (!ok)
    {
        std::cerr << "char event packet 1459 self-test failed\n";
    }
    return ok;
}
