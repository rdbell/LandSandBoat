#include "test_char_erase_entity_pop_3105.h"

#include "map/char_packet_queue_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "char ShouldEraseEntityUpdateOnPop 3105 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnPop erase formula for dual-wire cross-check (slice 3105):
//   packetType == 0x0D || packetType == 0x0E
auto inlineShouldEraseEntityUpdateOnPop(const std::uint16_t packetType) -> bool
{
    return packetType == 0x0D || packetType == 0x0E;
}

} // namespace

// Pure dual-wire expansion for charpacketqueuehelpers::ShouldEraseEntityUpdateOnPop
// (packetType == 0x0D || packetType == 0x0E; slice 3105).
auto runCharEraseEntityPop3105SelfTests() -> bool
{
    using charpacketqueuehelpers::ShouldClearPendingPositionOnPop;
    using charpacketqueuehelpers::ShouldEraseEntityUpdateOnPop;

    bool ok = true;

    const struct
    {
        std::uint16_t packetType;
        bool          want;
        const char*   label;
    } cases[] = {
        // Classic dual poles.
        { 0x0D, true, "0x0D admit" },
        { 0x0E, true, "0x0E admit" },

        // Reject poles.
        { 0x5B, false, "0x5B reject" },
        { 0x00, false, "type zero reject" },
        { 0x0C, false, "adjacent low reject" },
        { 0x0F, false, "adjacent high reject" },
        { 0x5A, false, "wrong type reject" },
        { 0x70, false, "synthesis type reject" },

        // Residual 2845 pins.
        { 0x0D, true, "residual 0x0D" },
        { 0x0E, true, "residual 0x0E" },
        { 0x5B, false, "residual 0x5B" },
        { 0x00, false, "residual type zero" },
        { 0x0C, false, "residual adjacent low" },
        { 0x0F, false, "residual adjacent high" },

        // Boundary neighbors around erase types.
        { 0x0B, false, "below 0x0C" },
        { 0x0C, false, "adjacent low of 0x0D" },
        { 0x0D, true, "exact char update" },
        { 0x0E, true, "exact entity update" },
        { 0x0F, false, "adjacent high of 0x0E" },
        { 0x10, false, "above 0x0F" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldEraseEntityUpdateOnPop(c.packetType);
        const bool inlineF = inlineShouldEraseEntityUpdateOnPop(c.packetType);
        const bool wantPin = c.packetType == 0x0D || c.packetType == 0x0E;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldEraseEntityUpdateOnPop dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldEraseEntityUpdateOnPop == pin formula") && ok;
    }

    // Pin composition: only 0x0D and 0x0E admit.
    ok = expect(ShouldEraseEntityUpdateOnPop(0x0D), "0x0D must erase") && ok;
    ok = expect(ShouldEraseEntityUpdateOnPop(0x0E), "0x0E must erase") && ok;
    ok = expect(!ShouldEraseEntityUpdateOnPop(0x5B), "0x5B must not erase") && ok;
    ok = expect(!ShouldEraseEntityUpdateOnPop(0x00), "type zero must not erase") && ok;

    // Dense compose over type neighborhood around 0x0D/0x0E.
    for (std::uint16_t packetType = 0x0A; packetType <= 0x12; ++packetType)
    {
        const bool got  = ShouldEraseEntityUpdateOnPop(packetType);
        const bool want = packetType == 0x0D || packetType == 0x0E;
        ok              = expect(got == want, "compose type neighborhood free == pin") && ok;
        ok              = expect(got == inlineShouldEraseEntityUpdateOnPop(packetType),
                    "compose type neighborhood free == inline") &&
             ok;
    }

    // Host-style inject poles: popPacket extracts entity id then calls free fn.
    // (Live CCharEntity::popPacket / OnPop is residual 2845 / packet queue tests.)
    struct HostPole
    {
        std::uint16_t packetType;
        bool          wantErase;
        const char*   label;
    };
    const HostPole poles[] = {
        { 0x0D, true, "host 0x0D → erase" },
        { 0x0E, true, "host 0x0E → erase" },
        { 0x5B, false, "host 0x5B → no erase" },
        { 0x00, false, "host type zero → no erase" },
        { 0x0C, false, "host adjacent low → no erase" },
        { 0x0F, false, "host adjacent high → no erase" },
        { 0x5A, false, "host wrong type → no erase" },
        { 0x70, false, "host synthesis type → no erase" },
    };
    for (const auto& p : poles)
    {
        const bool inject = p.packetType == 0x0D || p.packetType == 0x0E;
        ok                = expect(ShouldEraseEntityUpdateOnPop(p.packetType) == p.wantErase, p.label) && ok;
        ok                = expect(ShouldEraseEntityUpdateOnPop(p.packetType) == inject, "host inject dual-wire identity") &&
             ok;
        ok = expect(ShouldEraseEntityUpdateOnPop(p.packetType) == inlineShouldEraseEntityUpdateOnPop(p.packetType),
                    "host inject free == inline") &&
             ok;
    }

    // Production OnPop path semantics for erase branch:
    // 0x0D/0x0E → eraseEntityUpdate
    // 0x5B / other types → leave entity-update map unchanged on this gate
    ok = expect(ShouldEraseEntityUpdateOnPop(0x0D), "OnPop 0x0D → erase path") && ok;
    ok = expect(ShouldEraseEntityUpdateOnPop(0x0E), "OnPop 0x0E → erase path") && ok;
    ok = expect(!ShouldEraseEntityUpdateOnPop(0x5B), "OnPop 0x5B → no erase (clear-pending gate)") && ok;
    ok = expect(!ShouldEraseEntityUpdateOnPop(0x00), "OnPop other type → no erase") && ok;

    // Mutual exclusion with clear-pending (if / else if): erase admits never clear.
    ok = expect(!ShouldClearPendingPositionOnPop(0x0D, 99, 99), "erase 0x0D must not clear pending") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x0E, 99, 99), "erase 0x0E must not clear pending") && ok;
    ok = expect(ShouldClearPendingPositionOnPop(0x5B, 99, 99), "owner 0x5B must clear pending") && ok;
    ok = expect(!ShouldEraseEntityUpdateOnPop(0x5B), "0x5B must not erase") && ok;

    return ok;
}
