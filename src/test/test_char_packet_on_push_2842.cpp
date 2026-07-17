#include "test_char_packet_on_push_2842.h"

#include "map/char_packet_queue_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "char packet on push 2842 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runCharPacketOnPush2842SelfTests() -> bool
{
    using charpacketqueuehelpers::OnPush;
    using charpacketqueuehelpers::ShouldSetPendingPositionOnPush;

    bool ok = true;

    // Pure free function: owner 0x5B sets pending admission.
    ok = expect(ShouldSetPendingPositionOnPush(0x5B, 99, 99), "owner 0x5B admit") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x5B, 100, 99), "other entity reject") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x5A, 99, 99), "wrong type reject") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x5B, 0, 1), "zero entity vs owner") && ok;
    ok = expect(ShouldSetPendingPositionOnPush(0x5B, 0, 0), "zero owner match") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x00, 99, 99), "type zero reject") && ok;

    // Dual-wire OnPush template body invokes setPending(true) only when pure
    // gate admits.
    bool pending = false;
    OnPush(0x5B, 99, 99, [&](const bool value) { pending = value; });
    ok = expect(pending, "OnPush owner sets pending") && ok;

    pending = false;
    OnPush(0x5B, 100, 99, [&](const bool value) { pending = value; });
    ok = expect(!pending, "OnPush other entity no-op") && ok;

    pending = false;
    OnPush(0x5A, 99, 99, [&](const bool value) { pending = value; });
    ok = expect(!pending, "OnPush wrong type no-op") && ok;

    // Template still passes true (not false) when admitted.
    pending = false;
    bool sawFalse = false;
    OnPush(0x5B, 7, 7, [&](const bool value)
           {
               pending  = value;
               sawFalse = !value;
           });
    ok = expect(pending && !sawFalse, "OnPush invokes setPending(true)") && ok;

    return ok;
}
