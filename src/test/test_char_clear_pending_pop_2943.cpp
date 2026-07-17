#include "test_char_clear_pending_pop_2943.h"

#include "map/char_packet_queue_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "char ShouldClearPendingPositionOnPop 2943 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnPop clear-pending formula for dual-wire cross-check (slice 2943):
//   packetType == 0x5B && packetEntityID == ownerID
auto inlineShouldClearPendingPositionOnPop(const std::uint16_t packetType,
                                           const std::uint32_t packetEntityID,
                                           const std::uint32_t ownerID) -> bool
{
    return packetType == 0x5B && packetEntityID == ownerID;
}

} // namespace

// Pure dual-wire expansion for charpacketqueuehelpers::ShouldClearPendingPositionOnPop
// (packetType == 0x5B && packetEntityID == ownerID; slice 2943).
auto runCharClearPendingPop2943SelfTests() -> bool
{
    using charpacketqueuehelpers::ShouldClearPendingPositionOnPop;

    bool ok = true;

    const struct
    {
        std::uint16_t packetType;
        std::uint32_t packetEntityID;
        std::uint32_t ownerID;
        bool          want;
        const char*   label;
    } cases[] = {
        // Classic dual poles.
        { 0x5B, 99, 99, true, "owner 0x5B clears" },
        { 0x5B, 100, 99, false, "other entity 0x5B defers" },

        // Type / id extremes.
        { 0x5A, 99, 99, false, "wrong type rejects" },
        { 0x0D, 99, 99, false, "erase type rejects clear" },
        { 0x0E, 99, 99, false, "erase type 0x0E rejects clear" },
        { 0x00, 99, 99, false, "type zero rejects" },
        { 0x5B, 0, 1, false, "zero entity vs owner rejects" },
        { 0x5B, 0, 0, true, "zero owner match clears" },

        // Residual 2845 pins.
        { 0x5B, 99, 99, true, "residual owner 0x5B" },
        { 0x5B, 100, 99, false, "residual other entity" },
        { 0x5A, 99, 99, false, "residual wrong type" },
        { 0x0D, 99, 99, false, "residual erase type" },

        // Boundary neighbors around type 0x5B.
        { 0x5A, 7, 7, false, "adjacent low type" },
        { 0x5B, 7, 7, true, "exact position type owner match" },
        { 0x5C, 7, 7, false, "adjacent high type" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldClearPendingPositionOnPop(c.packetType, c.packetEntityID, c.ownerID);
        const bool inlineF = inlineShouldClearPendingPositionOnPop(c.packetType, c.packetEntityID, c.ownerID);
        const bool wantPin = c.packetType == 0x5B && c.packetEntityID == c.ownerID;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldClearPendingPositionOnPop dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldClearPendingPositionOnPop == pin formula") && ok;
    }

    // Pin composition: type 0x5B AND entity == owner only.
    ok = expect(ShouldClearPendingPositionOnPop(0x5B, 1, 1), "0x5B owner match must clear") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x5B, 1, 2), "0x5B other entity must not clear") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x5A, 1, 1), "non-0x5B must not clear") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x0D, 1, 1), "erase type must not clear") && ok;

    // Dense compose over type neighborhood around 0x5B with matching ids.
    for (std::uint16_t packetType = 0x58; packetType <= 0x5E; ++packetType)
    {
        const bool got  = ShouldClearPendingPositionOnPop(packetType, 42, 42);
        const bool want = packetType == 0x5B;
        ok              = expect(got == want, "compose type neighborhood free == pin") && ok;
        ok              = expect(got == inlineShouldClearPendingPositionOnPop(packetType, 42, 42),
                    "compose type neighborhood free == inline") &&
             ok;
    }

    // Dense compose: matching vs non-matching entity for fixed 0x5B.
    for (std::uint32_t entityID = 0; entityID < 8; ++entityID)
    {
        constexpr std::uint32_t ownerID = 3;
        const bool              got     = ShouldClearPendingPositionOnPop(0x5B, entityID, ownerID);
        const bool              want    = entityID == ownerID;
        ok                              = expect(got == want, "compose entity match free == pin") && ok;
        ok                              = expect(got == inlineShouldClearPendingPositionOnPop(0x5B, entityID, ownerID),
                    "compose entity match free == inline") &&
             ok;
    }

    // Host-style inject poles: popPacket extracts entity id then calls free fn.
    // (Live CCharEntity::popPacket / OnPop is residual 2845 / packet queue tests.)
    struct HostPole
    {
        std::uint16_t packetType;
        std::uint32_t packetEntityID;
        std::uint32_t ownerID;
        bool          wantClear;
        const char*   label;
    };
    const HostPole poles[] = {
        { 0x5B, 99, 99, true, "host owner 0x5B → clear" },
        { 0x5B, 100, 99, false, "host other 0x5B → no clear" },
        { 0x0D, 99, 99, false, "host erase type → no clear" },
        { 0x5A, 99, 99, false, "host wrong type → no clear" },
        { 0x5B, 0, 0, true, "host zero match → clear" },
        { 0x5B, 0, 1, false, "host zero mismatch → no clear" },
    };
    for (const auto& p : poles)
    {
        const bool inject = p.packetType == 0x5B && p.packetEntityID == p.ownerID;
        ok                = expect(ShouldClearPendingPositionOnPop(p.packetType, p.packetEntityID, p.ownerID) == p.wantClear,
                    p.label) &&
             ok;
        ok = expect(ShouldClearPendingPositionOnPop(p.packetType, p.packetEntityID, p.ownerID) == inject,
                    "host inject dual-wire identity") &&
             ok;
        ok = expect(ShouldClearPendingPositionOnPop(p.packetType, p.packetEntityID, p.ownerID) ==
                        inlineShouldClearPendingPositionOnPop(p.packetType, p.packetEntityID, p.ownerID),
                    "host inject free == inline") &&
             ok;
    }

    // Production OnPop path semantics for clear-pending branch:
    // owner 0x5B → setPending(false)
    // other entity / wrong type / erase types → leave pending unchanged on this gate
    ok = expect(ShouldClearPendingPositionOnPop(0x5B, 7, 7), "OnPop owner 0x5B → clear path") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x5B, 8, 7), "OnPop other 0x5B → no clear") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x0D, 7, 7), "OnPop erase → no clear (mutual exclusion)") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x00, 7, 7), "OnPop other type → no clear") && ok;

    return ok;
}
