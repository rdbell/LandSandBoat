#include "test_charentity_clear_pending_pop_3179.h"

#include "map/char_packet_queue_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "charentity ShouldClearPendingPositionOnPop 3179 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnPop clear-pending formula for dual-wire cross-check (dedicated slice 3179):
//   packetType == 0x5B && packetEntityID == ownerID
auto inlineShouldClearPendingPositionOnPop(const std::uint16_t packetType,
                                           const std::uint32_t packetEntityID,
                                           const std::uint32_t ownerID) -> bool
{
    return packetType == 0x5B && packetEntityID == ownerID;
}

// Compact dual-wire pin matching Go pinShouldClearPendingPositionOnPop3179 /
// C++ capacity:
//   packetType == 0x5B && packetEntityID == ownerID
auto pinShouldClearPendingPositionOnPop(const std::uint16_t packetType,
                                        const std::uint32_t packetEntityID,
                                        const std::uint32_t ownerID) -> bool
{
    return packetType == 0x5B && packetEntityID == ownerID;
}

} // namespace

// Pure dual-wire expansion for charpacketqueuehelpers::ShouldClearPendingPositionOnPop
// (packetType == 0x5B && packetEntityID == ownerID; dedicated slice 3179
// expand residual 2943 / pure 2845).
//
// Coverage:
//   - free == inline == pin == (packetType == 0x5B && packetEntityID == ownerID)
//   - residual 2845 / 2943 pins still hold
//   - host-style inject poles + mutual exclusion with erase types
auto runCharentityClearPendingPop3179SelfTests() -> bool
{
    using charpacketqueuehelpers::ShouldClearPendingPositionOnPop;

    bool ok = true;

    // Residual 2845 / 2943 pins still hold under dual-wire.
    ok = expect(ShouldClearPendingPositionOnPop(0x5B, 99, 99), "residual 2943 owner 0x5B → clear") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x5B, 100, 99), "residual 2943 other entity → no clear") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x5A, 99, 99), "residual 2943 wrong type → no clear") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x0D, 99, 99), "residual 2943/2845 erase type → no clear") && ok;
    ok = expect(ShouldClearPendingPositionOnPop(0x5B, 0, 0), "residual 2943 zero owner match → clear") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x5B, 0, 1), "residual 2943 zero entity vs owner → no clear") && ok;

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

        // Residual 2943 / 2845 pins.
        { 0x5B, 99, 99, true, "residual 2943 owner 0x5B" },
        { 0x5B, 100, 99, false, "residual 2943 other entity" },
        { 0x5A, 99, 99, false, "residual 2943 wrong type" },
        { 0x0D, 99, 99, false, "residual 2845 erase type" },

        // Boundary neighbors around type 0x5B.
        { 0x5A, 7, 7, false, "adjacent low type" },
        { 0x5B, 7, 7, true, "exact position type owner match" },
        { 0x5C, 7, 7, false, "adjacent high type" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldClearPendingPositionOnPop(c.packetType, c.packetEntityID, c.ownerID);
        const bool inlineF = inlineShouldClearPendingPositionOnPop(c.packetType, c.packetEntityID, c.ownerID);
        const bool pinF    = pinShouldClearPendingPositionOnPop(c.packetType, c.packetEntityID, c.ownerID);
        const bool wantPin = c.packetType == 0x5B && c.packetEntityID == c.ownerID;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldClearPendingPositionOnPop dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldClearPendingPositionOnPop dual-wire == pin formula") && ok;
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
        ok = expect(got == pinShouldClearPendingPositionOnPop(packetType, 42, 42),
                    "compose type neighborhood free == pin helper") &&
             ok;
    }

    // Dense compose: matching vs non-matching entity for fixed 0x5B.
    constexpr std::uint32_t ownerID = 3;
    for (std::uint32_t entityID = 0; entityID < 8; ++entityID)
    {
        const bool got  = ShouldClearPendingPositionOnPop(0x5B, entityID, ownerID);
        const bool want = entityID == ownerID;
        ok              = expect(got == want, "compose entity free == pin") && ok;
        ok              = expect(got == inlineShouldClearPendingPositionOnPop(0x5B, entityID, ownerID),
                    "compose entity free == inline") &&
             ok;
        ok = expect(got == pinShouldClearPendingPositionOnPop(0x5B, entityID, ownerID),
                    "compose entity free == pin helper") &&
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
        { 0x0E, 99, 99, false, "host erase 0x0E → no clear" },
        { 0x00, 99, 99, false, "host type zero → no clear" },
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
        ok = expect(ShouldClearPendingPositionOnPop(p.packetType, p.packetEntityID, p.ownerID) ==
                        pinShouldClearPendingPositionOnPop(p.packetType, p.packetEntityID, p.ownerID),
                    "host inject free == pin") &&
             ok;
    }

    // Production OnPop path semantics for clear-pending branch:
    // owner 0x5B → setPending(false)
    // other entity / wrong type / erase types → leave pending unchanged on this gate
    ok = expect(ShouldClearPendingPositionOnPop(0x5B, 7, 7), "OnPop owner 0x5B → clear path") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x5B, 8, 7), "OnPop other 0x5B → no clear") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x0D, 7, 7), "OnPop erase → no clear (mutual exclusion)") && ok;
    ok = expect(!ShouldClearPendingPositionOnPop(0x00, 7, 7), "OnPop other type → no clear") && ok;

    // Erase types never clear pending on this gate (mutual exclusion with 3105 erase).
    for (const std::uint16_t typ : { std::uint16_t{ 0x0D }, std::uint16_t{ 0x0E } })
    {
        ok = expect(!ShouldClearPendingPositionOnPop(typ, 99, 99), "erase type must not clear pending") && ok;
        ok = expect(!inlineShouldClearPendingPositionOnPop(typ, 99, 99), "erase type inline must not clear") && ok;
        ok = expect(!pinShouldClearPendingPositionOnPop(typ, 99, 99), "erase type pin must not clear") && ok;
    }

    return ok;
}
