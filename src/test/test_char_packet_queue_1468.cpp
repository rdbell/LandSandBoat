#include "test_char_packet_queue_1468.h"

#include "map/char_packet_queue_capacity.h"

#include <cstdint>
#include <iostream>

auto runCharPacketQueue1468SelfTests() -> bool
{
    bool ok = charpacketqueuehelpers::Filtered(0x70, true) &&
              !charpacketqueuehelpers::Filtered(0x70, false) &&
              !charpacketqueuehelpers::Filtered(0x6F, true);

    bool pending = false;
    charpacketqueuehelpers::OnPush(0x5B, 99, 99, [&](const bool value) { pending = value; });
    ok = pending && ok;
    pending = false;
    charpacketqueuehelpers::OnPush(0x5B, 100, 99, [&](const bool value) { pending = value; });
    ok = !pending && ok;
    charpacketqueuehelpers::OnPush(0x5A, 99, 99, [&](const bool value) { pending = value; });
    ok = !pending && ok;

    std::uint32_t erased = 0;
    charpacketqueuehelpers::OnPop(
        0x0D, 42, 99, [&](const std::uint32_t id) { erased = id; }, [&](const bool value) { pending = value; });
    ok = erased == 42 && !pending && ok;
    erased = 0;
    charpacketqueuehelpers::OnPop(
        0x0E, 43, 99, [&](const std::uint32_t id) { erased = id; }, [&](const bool value) { pending = value; });
    ok = erased == 43 && !pending && ok;

    pending = true;
    charpacketqueuehelpers::OnPop(
        0x5B, 99, 99, [&](const std::uint32_t id) { erased = id; }, [&](const bool value) { pending = value; });
    ok = !pending && erased == 43 && ok;
    pending = true;
    charpacketqueuehelpers::OnPop(
        0x5B, 100, 99, [&](const std::uint32_t id) { erased = id; }, [&](const bool value) { pending = value; });
    ok = pending && erased == 43 && ok;

    if (!ok)
    {
        std::cerr << "char packet queue 1468 self-test failed\n";
    }
    return ok;
}
