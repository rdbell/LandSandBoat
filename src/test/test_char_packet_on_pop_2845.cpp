#include "test_char_packet_on_pop_2845.h"

#include "map/char_packet_queue_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "char packet on pop 2845 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runCharPacketOnPop2845SelfTests() -> bool
{
    using charpacketqueuehelpers::OnPop;
    using charpacketqueuehelpers::ShouldClearPendingPositionOnPop;
    using charpacketqueuehelpers::ShouldEraseEntityUpdateOnPop;

    bool ok = true;

    // Pure free function: entity-update types admit erase.
    ok = expect(ShouldEraseEntityUpdateOnPop(0x0D), "0x0D erase admit") && ok;
    ok = expect(ShouldEraseEntityUpdateOnPop(0x0E), "0x0E erase admit") && ok;
    ok = expect(!ShouldEraseEntityUpdateOnPop(0x5B), "0x5B erase reject") && ok;
    ok = expect(!ShouldEraseEntityUpdateOnPop(0x00), "type zero erase reject") && ok;
    ok = expect(!ShouldEraseEntityUpdateOnPop(0x0C), "adjacent type erase reject") && ok;
    ok = expect(!ShouldEraseEntityUpdateOnPop(0x0F), "adjacent type erase reject high") && ok;

    // Pure free function: owner 0x5B clears pending admission.
    ok = expect(ShouldClearPendingPositionOnPop(0x5B, 99, 99), "owner 0x5B clear admit") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x5B, 100, 99), "other entity clear reject") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x5A, 99, 99), "wrong type clear reject") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x0D, 99, 99), "erase type clear reject") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x5B, 0, 1), "zero entity vs owner clear") && ok;
    ok = expect(ShouldClearPendingPositionOnPop(0x5B, 0, 0), "zero owner match clear") && ok;

    // Dual-wire OnPop: erase path for 0x0D / 0x0E.
    std::uint32_t erased = 0;
    bool          pending = true;
    OnPop(
        0x0D, 42, 99, [&](const std::uint32_t id) { erased = id; }, [&](const bool value) { pending = value; });
    ok = expect(erased == 42 && pending, "OnPop 0x0D erases only") && ok;

    erased  = 0;
    pending = true;
    OnPop(
        0x0E, 43, 99, [&](const std::uint32_t id) { erased = id; }, [&](const bool value) { pending = value; });
    ok = expect(erased == 43 && pending, "OnPop 0x0E erases only") && ok;

    // Dual-wire OnPop: clear-pending for owner 0x5B; no erase.
    erased  = 0;
    pending = true;
    OnPop(
        0x5B, 99, 99, [&](const std::uint32_t id) { erased = id; }, [&](const bool value) { pending = value; });
    ok = expect(!pending && erased == 0, "OnPop owner 0x5B clears pending") && ok;

    pending = true;
    OnPop(
        0x5B, 100, 99, [&](const std::uint32_t id) { erased = id; }, [&](const bool value) { pending = value; });
    ok = expect(pending && erased == 0, "OnPop other entity 0x5B no-op") && ok;

    pending = true;
    OnPop(
        0x5A, 99, 99, [&](const std::uint32_t id) { erased = id; }, [&](const bool value) { pending = value; });
    ok = expect(pending && erased == 0, "OnPop wrong type no-op") && ok;

    // Template still passes false (not true) when clear-pending admitted.
    pending       = true;
    bool sawTrue  = false;
    OnPop(
        0x5B, 7, 7,
        [&](const std::uint32_t id) { erased = id; },
        [&](const bool value)
        {
            pending = value;
            sawTrue = value;
        });
    ok = expect(!pending && !sawTrue && erased == 0, "OnPop invokes setPending(false)") && ok;

    // if/else if mutual exclusion: erase types never also clear pending.
    pending = true;
    erased  = 0;
    OnPop(
        0x0D, 99, 99, [&](const std::uint32_t id) { erased = id; }, [&](const bool value) { pending = value; });
    ok = expect(erased == 99 && pending, "OnPop erase excludes clear-pending") && ok;

    return ok;
}
