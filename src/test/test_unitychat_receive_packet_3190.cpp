#include "test_unitychat_receive_packet_3190.h"

#include "map/unitychat_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "unitychat ShouldReceiveUnityPacket 3190 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline PushPacket receive filter for dual-wire cross-check (slice 3190):
//   !isSender && !isDisappear && !inPrison
auto inlineShouldReceiveUnityPacket3190(const bool isSender, const bool isDisappear, const bool inPrison) -> bool
{
    return !isSender && !isDisappear && !inPrison;
}

// Positive if/else dual-wire pin matching free / capacity body (slice 3190).
// Positive form only — avoid De Morgan rewrites of !a && !b && !c.
auto pinShouldReceiveUnityPacket3190(const bool isSender, const bool isDisappear, const bool inPrison) -> bool
{
    if (isSender)
    {
        return false;
    }
    if (isDisappear)
    {
        return false;
    }
    if (inPrison)
    {
        return false;
    }
    return true;
}

} // namespace

// Pure dual-wire expansion for unitychathelpers::ShouldReceiveUnityPacket
// (!isSender && !isDisappear && !inPrison; dedicated slice 3190; residual
// expand 2933 / pure 1356).
//
// Coverage:
//   - free == inline == pin (positive if/else)
//   - residual 2933 / 1356 pins still hold
//   - dense 2³ free == inline == pin
//   - host inject PushPacket path semantics
auto runUnitychatReceivePacket3190SelfTests() -> bool
{
    using unitychathelpers::ShouldReceiveUnityPacket;

    bool ok = true;

    // Residual 1356 / 2933 pins still hold under dedicated dual-wire.
    ok = expect(ShouldReceiveUnityPacket(false, false, false), "residual 1356/2933: eligible member must receive") && ok;
    ok = expect(!ShouldReceiveUnityPacket(true, false, false), "residual 1356/2933: sender must skip") && ok;
    ok = expect(!ShouldReceiveUnityPacket(false, true, false), "residual 1356/2933: disappear must skip") && ok;
    ok = expect(!ShouldReceiveUnityPacket(false, false, true), "residual 1356/2933: prison must skip") && ok;

    // Core poles: free == inline == pin (positive if/else).
    const struct
    {
        bool        isSender;
        bool        isDisappear;
        bool        inPrison;
        bool        want;
        const char* label;
    } cases[] = {
        // All filters clear → receive
        { false, false, false, true, "eligible member receives" },

        // Any filter true → skip
        { true, false, false, false, "sender skips" },
        { false, true, false, false, "disappear skips" },
        { false, false, true, false, "prison skips" },
        { true, true, false, false, "sender+disappear skips" },
        { true, false, true, false, "sender+prison skips" },
        { false, true, true, false, "disappear+prison skips" },
        { true, true, true, false, "all filters set skips" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldReceiveUnityPacket(c.isSender, c.isDisappear, c.inPrison);
        const bool inlineF = inlineShouldReceiveUnityPacket3190(c.isSender, c.isDisappear, c.inPrison);
        const bool pin     = pinShouldReceiveUnityPacket3190(c.isSender, c.isDisappear, c.inPrison);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReceiveUnityPacket dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "ShouldReceiveUnityPacket == positive if/else pin") && ok;
    }

    // Free == pin across residual poles (positive if/else pin form).
    ok = expect(ShouldReceiveUnityPacket(false, false, false) == pinShouldReceiveUnityPacket3190(false, false, false),
                "free == pin residual eligible receive") &&
         ok;
    ok = expect(ShouldReceiveUnityPacket(true, false, false) == pinShouldReceiveUnityPacket3190(true, false, false),
                "free == pin residual sender skip") &&
         ok;
    ok = expect(ShouldReceiveUnityPacket(false, true, false) == pinShouldReceiveUnityPacket3190(false, true, false),
                "free == pin residual disappear skip") &&
         ok;
    ok = expect(ShouldReceiveUnityPacket(false, false, true) == pinShouldReceiveUnityPacket3190(false, false, true),
                "free == pin residual prison skip") &&
         ok;

    // Dense compose: full 2^3 boolean space free == inline == pin.
    for (const bool isSender : { false, true })
    {
        for (const bool isDisappear : { false, true })
        {
            for (const bool inPrison : { false, true })
            {
                const bool got     = ShouldReceiveUnityPacket(isSender, isDisappear, inPrison);
                const bool inlineF = inlineShouldReceiveUnityPacket3190(isSender, isDisappear, inPrison);
                const bool pin     = pinShouldReceiveUnityPacket3190(isSender, isDisappear, inPrison);
                ok                 = expect(got == pin, "compose free == positive if/else pin") && ok;
                ok                 = expect(got == inlineF, "compose free == inline") && ok;
            }
        }
    }

    // Host-style compose: id equality / disappear / prison injects (PushPacket).
    const uint32 senderID = 100;
    const struct
    {
        uint32      id;
        bool        disappear;
        bool        inPrison;
        bool        wantReceive;
        const char* label;
    } members[] = {
        { 100, false, false, false, "self sender skips" },
        { 101, false, false, true, "other online receives" },
        { 102, true, false, false, "other disappear skips" },
        { 103, false, true, false, "other prison skips" },
        { 100, true, true, false, "sender disappear prison skips" },
        { 104, true, true, false, "other disappear+prison skips" },
    };

    for (const auto& m : members)
    {
        const bool isSender = m.id == senderID;
        const bool got      = ShouldReceiveUnityPacket(isSender, m.disappear, m.inPrison);
        const bool inlineF  = inlineShouldReceiveUnityPacket3190(isSender, m.disappear, m.inPrison);
        const bool pin      = pinShouldReceiveUnityPacket3190(isSender, m.disappear, m.inPrison);
        ok                  = expect(got == m.wantReceive, m.label) && ok;
        ok                  = expect(got == inlineF && got == pin, "host compose free == inline == pin") && ok;
    }

    return ok;
}
