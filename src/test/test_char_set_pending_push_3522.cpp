#include "test_char_set_pending_push_3522.h"

#include "map/char_packet_queue_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "char ShouldSetPendingPositionOnPush 3522 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnPush pending-position formula for dual-wire cross-check (dedicated slice 3522):
//   packetType == 0x5B && packetEntityID == ownerID
auto inlineShouldSetPendingPositionOnPush(const std::uint16_t packetType,
                                          const std::uint32_t packetEntityID,
                                          const std::uint32_t ownerID) -> bool
{
    return packetType == 0x5B && packetEntityID == ownerID;
}

// Compact dual-wire pin matching Go pinShouldSetPendingPositionOnPush3522 /
// C++ capacity:
//   packetType == 0x5B && packetEntityID == ownerID
auto pinShouldSetPendingPositionOnPush(const std::uint16_t packetType,
                                       const std::uint32_t packetEntityID,
                                       const std::uint32_t ownerID) -> bool
{
    return packetType == 0x5B && packetEntityID == ownerID;
}

// Prior dedicated 3446 pin (retained under expand 3522).
auto pinShouldSetPendingPositionOnPush3446(const std::uint16_t packetType,
                                           const std::uint32_t packetEntityID,
                                           const std::uint32_t ownerID) -> bool
{
    return packetType == 0x5B && packetEntityID == ownerID;
}

// Prior dedicated 3395 pin (retained under expand 3522).
auto pinShouldSetPendingPositionOnPush3395(const std::uint16_t packetType,
                                           const std::uint32_t packetEntityID,
                                           const std::uint32_t ownerID) -> bool
{
    return packetType == 0x5B && packetEntityID == ownerID;
}

} // namespace

// Pure dual-wire expansion for charpacketqueuehelpers::ShouldSetPendingPositionOnPush
// (packetType == 0x5B && packetEntityID == ownerID; dedicated slice 3522
// expand residual 3125; prior dedicated 3446 / 3395 / pure 2842).
//
// Coverage:
//   - free == inline == pin == (packetType == 0x5B && packetEntityID == ownerID)
//   - residual 2842 / 3125 + prior dedicated 3446 / 3395 pins still hold
//   - host-style inject poles + OnPush semantics
//   - sibling clear-pending / erase left alone (do not thrash clear_pending_pop)
auto runCharSetPendingPush3522SelfTests() -> bool
{
    using charpacketqueuehelpers::OnPush;
    using charpacketqueuehelpers::ShouldClearPendingPositionOnPop;
    using charpacketqueuehelpers::ShouldEraseEntityUpdateOnPop;
    using charpacketqueuehelpers::ShouldSetPendingPositionOnPush;

    bool ok = true;

    // Residual 2842 / 3125 / prior dedicated 3446 / 3395 pins still hold under dual-wire.
    ok = expect(ShouldSetPendingPositionOnPush(0x5B, 99, 99), "residual 3125 owner 0x5B → admit") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x5B, 100, 99), "residual 3125 other entity → reject") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x5A, 99, 99), "residual 3125 wrong type → reject") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x00, 99, 99), "residual 3125 type zero → reject") && ok;
    ok = expect(ShouldSetPendingPositionOnPush(0x5B, 0, 0), "residual 3125 zero owner match → admit") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x5B, 0, 1), "residual 3125 zero entity vs owner → reject") && ok;

    const struct
    {
        std::uint16_t packetType;
        std::uint32_t packetEntityID;
        std::uint32_t ownerID;
        bool          want;
        const char*   label;
    } cases[] = {
        // Classic dual poles: 0x5B match / mismatch.
        { 0x5B, 99, 99, true, "owner 0x5B admit" },
        { 0x5B, 100, 99, false, "other entity 0x5B reject" },

        // Other packet types reject even with matching entity/owner.
        { 0x5A, 99, 99, false, "wrong type reject" },
        { 0x00, 99, 99, false, "type zero reject" },
        { 0x0D, 99, 99, false, "erase type reject" },
        { 0x0E, 99, 99, false, "entity update type reject" },
        { 0x70, 99, 99, false, "synthesis type reject" },

        // Id extremes.
        { 0x5B, 0, 1, false, "zero entity vs owner reject" },
        { 0x5B, 0, 0, true, "zero owner match admit" },

        // Residual 3125 / 2842 pins.
        { 0x5B, 99, 99, true, "residual 3125 owner 0x5B" },
        { 0x5B, 100, 99, false, "residual 3125 other entity" },
        { 0x5A, 99, 99, false, "residual 3125 wrong type" },
        { 0x5B, 0, 1, false, "residual 3125 zero entity vs owner" },
        { 0x5B, 0, 0, true, "residual 3125 zero owner match" },
        { 0x00, 99, 99, false, "residual 3125 type zero" },

        // Prior dedicated 3446 re-pins.
        { 0x5B, 99, 99, true, "prior 3446 owner 0x5B" },
        { 0x5B, 100, 99, false, "prior 3446 other entity" },
        { 0x5A, 99, 99, false, "prior 3446 wrong type" },
        { 0x5B, 0, 0, true, "prior 3446 zero owner match" },

        // Prior dedicated 3395 re-pins.
        { 0x5B, 99, 99, true, "prior 3395 owner 0x5B" },
        { 0x5B, 100, 99, false, "prior 3395 other entity" },
        { 0x5A, 99, 99, false, "prior 3395 wrong type" },
        { 0x5B, 0, 0, true, "prior 3395 zero owner match" },

        // Boundary neighbors around type 0x5B.
        { 0x5A, 7, 7, false, "adjacent low type" },
        { 0x5B, 7, 7, true, "exact position type owner match" },
        { 0x5C, 7, 7, false, "adjacent high type" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSetPendingPositionOnPush(c.packetType, c.packetEntityID, c.ownerID);
        const bool inlineF = inlineShouldSetPendingPositionOnPush(c.packetType, c.packetEntityID, c.ownerID);
        const bool pinF    = pinShouldSetPendingPositionOnPush(c.packetType, c.packetEntityID, c.ownerID);
        const bool pin3446 = pinShouldSetPendingPositionOnPush3446(c.packetType, c.packetEntityID, c.ownerID);
        const bool pin3395 = pinShouldSetPendingPositionOnPush3395(c.packetType, c.packetEntityID, c.ownerID);
        const bool wantPin = c.packetType == 0x5B && c.packetEntityID == c.ownerID;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSetPendingPositionOnPush dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldSetPendingPositionOnPush dual-wire == pin formula") && ok;
        ok = expect(got == pin3446, "ShouldSetPendingPositionOnPush dual-wire == prior pin3446") && ok;
        ok = expect(got == pin3395, "ShouldSetPendingPositionOnPush dual-wire == prior pin3395") && ok;
        ok = expect(got == wantPin, "ShouldSetPendingPositionOnPush == pin formula") && ok;
    }

    // Pin composition: type 0x5B AND entity == owner only.
    ok = expect(ShouldSetPendingPositionOnPush(0x5B, 1, 1), "0x5B owner match must admit") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x5B, 1, 2), "0x5B other entity must reject") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x5A, 1, 1), "non-0x5B must reject") && ok;
    ok = expect(!ShouldSetPendingPositionOnPush(0x00, 1, 1), "type zero must reject") && ok;

    // Dense compose over type neighborhood around 0x5B with matching ids.
    for (std::uint16_t packetType = 0x58; packetType <= 0x5E; ++packetType)
    {
        const bool got  = ShouldSetPendingPositionOnPush(packetType, 42, 42);
        const bool want = packetType == 0x5B;
        ok              = expect(got == want, "compose type neighborhood free == pin") && ok;
        ok              = expect(got == inlineShouldSetPendingPositionOnPush(packetType, 42, 42),
                    "compose type neighborhood free == inline") &&
             ok;
        ok = expect(got == pinShouldSetPendingPositionOnPush(packetType, 42, 42),
                    "compose type neighborhood free == pin helper") &&
             ok;
        ok = expect(got == pinShouldSetPendingPositionOnPush3446(packetType, 42, 42),
                    "compose type neighborhood free == prior pin3446") &&
             ok;
        ok = expect(got == pinShouldSetPendingPositionOnPush3395(packetType, 42, 42),
                    "compose type neighborhood free == prior pin3395") &&
             ok;
    }

    // Dense compose: matching vs non-matching entity for fixed 0x5B.
    constexpr std::uint32_t ownerID = 3;
    for (std::uint32_t entityID = 0; entityID < 8; ++entityID)
    {
        const bool got  = ShouldSetPendingPositionOnPush(0x5B, entityID, ownerID);
        const bool want = entityID == ownerID;
        ok              = expect(got == want, "compose entity free == pin") && ok;
        ok              = expect(got == inlineShouldSetPendingPositionOnPush(0x5B, entityID, ownerID),
                    "compose entity free == inline") &&
             ok;
        ok = expect(got == pinShouldSetPendingPositionOnPush(0x5B, entityID, ownerID),
                    "compose entity free == pin helper") &&
             ok;
        ok = expect(got == pinShouldSetPendingPositionOnPush3446(0x5B, entityID, ownerID),
                    "compose entity free == prior pin3446") &&
             ok;
        ok = expect(got == pinShouldSetPendingPositionOnPush3395(0x5B, entityID, ownerID),
                    "compose entity free == prior pin3395") &&
             ok;
    }

    // Host-style inject poles: pushPacket extracts entity id then calls free fn.
    // (Live CCharEntity::pushPacket / OnPush is residual 2842 / packet queue tests.)
    struct HostPole
    {
        std::uint16_t packetType;
        std::uint32_t packetEntityID;
        std::uint32_t ownerID;
        bool          wantAdmit;
        const char*   label;
    };
    const HostPole poles[] = {
        { 0x5B, 99, 99, true, "host owner 0x5B → admit" },
        { 0x5B, 100, 99, false, "host other entity → reject" },
        { 0x5A, 99, 99, false, "host wrong type → reject" },
        { 0x00, 99, 99, false, "host type zero → reject" },
        { 0x5B, 0, 0, true, "host zero owner match → admit" },
        { 0x5B, 0, 1, false, "host zero entity vs owner → reject" },
        { 0x0D, 99, 99, false, "host erase type → reject" },
        { 0x70, 99, 99, false, "host synthesis type → reject" },
    };
    for (const auto& p : poles)
    {
        const bool inject = p.packetType == 0x5B && p.packetEntityID == p.ownerID;
        ok                = expect(ShouldSetPendingPositionOnPush(p.packetType, p.packetEntityID, p.ownerID) == p.wantAdmit,
                    p.label) &&
             ok;
        ok = expect(ShouldSetPendingPositionOnPush(p.packetType, p.packetEntityID, p.ownerID) == inject,
                    "host inject dual-wire identity") &&
             ok;
        ok = expect(ShouldSetPendingPositionOnPush(p.packetType, p.packetEntityID, p.ownerID) ==
                        inlineShouldSetPendingPositionOnPush(p.packetType, p.packetEntityID, p.ownerID),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldSetPendingPositionOnPush(p.packetType, p.packetEntityID, p.ownerID) ==
                        pinShouldSetPendingPositionOnPush(p.packetType, p.packetEntityID, p.ownerID),
                    "host inject free == pin") &&
             ok;
        ok = expect(ShouldSetPendingPositionOnPush(p.packetType, p.packetEntityID, p.ownerID) ==
                        pinShouldSetPendingPositionOnPush3446(p.packetType, p.packetEntityID, p.ownerID),
                    "host inject free == prior pin3446") &&
             ok;
        ok = expect(ShouldSetPendingPositionOnPush(p.packetType, p.packetEntityID, p.ownerID) ==
                        pinShouldSetPendingPositionOnPush3395(p.packetType, p.packetEntityID, p.ownerID),
                    "host inject free == prior pin3395") &&
             ok;
    }

    // Production OnPush path semantics:
    // owner 0x5B → setPending(true); other entity / wrong type → no-op.
    bool pending = false;
    OnPush(0x5B, 99, 99, [&](const bool value) { pending = value; });
    ok = expect(pending, "OnPush owner sets pending") && ok;

    pending = false;
    OnPush(0x5B, 100, 99, [&](const bool value) { pending = value; });
    ok = expect(!pending, "OnPush other entity no-op") && ok;

    pending = false;
    OnPush(0x5A, 99, 99, [&](const bool value) { pending = value; });
    ok = expect(!pending, "OnPush wrong type no-op") && ok;

    // Sibling free functions left alone (same scalar formula for clear-pending;
    // erase never admits 0x5B). Do not thrash clear_pending_pop.
    ok = expect(ShouldClearPendingPositionOnPop(0x5B, 99, 99), "sibling clear-pending owner admit") && ok;
    ok = expect(!ShouldEraseEntityUpdateOnPop(0x5B), "sibling erase must not admit 0x5B") && ok;

    return ok;
}
