#include "test_unitychat_receive_packet_2933.h"

#include "map/unitychat_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "unitychat ShouldReceiveUnityPacket 2933 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline PushPacket receive filter for dual-wire cross-check (slice 2933):
//   !isSender && !isDisappear && !inPrison
auto inlineShouldReceiveUnityPacket(const bool isSender, const bool isDisappear, const bool inPrison) -> bool
{
    return !isSender && !isDisappear && !inPrison;
}

} // namespace

// Pure dual-wire expansion for unitychathelpers::ShouldReceiveUnityPacket
// (!isSender && !isDisappear && !inPrison; slice 2933).
auto runUnitychatReceivePacket2933SelfTests() -> bool
{
    using unitychathelpers::ShouldReceiveUnityPacket;

    bool ok = true;

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
        const bool inlineF = inlineShouldReceiveUnityPacket(c.isSender, c.isDisappear, c.inPrison);
        const bool wantPin = !c.isSender && !c.isDisappear && !c.inPrison;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReceiveUnityPacket dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldReceiveUnityPacket == pin formula") && ok;
    }

    // Pin composition: all three filters must be clear.
    ok = expect(ShouldReceiveUnityPacket(false, false, false), "all-clear must receive") && ok;
    ok = expect(!ShouldReceiveUnityPacket(true, false, false), "sender must not receive") && ok;
    ok = expect(!ShouldReceiveUnityPacket(false, true, false), "disappear must not receive") && ok;
    ok = expect(!ShouldReceiveUnityPacket(false, false, true), "prison must not receive") && ok;

    // Dense compose: full 2^3 boolean space.
    for (const bool isSender : { false, true })
    {
        for (const bool isDisappear : { false, true })
        {
            for (const bool inPrison : { false, true })
            {
                const bool got  = ShouldReceiveUnityPacket(isSender, isDisappear, inPrison);
                const bool want = !isSender && !isDisappear && !inPrison;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldReceiveUnityPacket(isSender, isDisappear, inPrison),
                           "compose free == inline") &&
                     ok;
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
    };

    for (const auto& m : members)
    {
        const bool isSender = m.id == senderID;
        const bool got      = ShouldReceiveUnityPacket(isSender, m.disappear, m.inPrison);
        ok                  = expect(got == m.wantReceive, m.label) && ok;
        ok                  = expect(got == inlineShouldReceiveUnityPacket(isSender, m.disappear, m.inPrison),
                    "host compose free == inline") &&
             ok;
    }

    return ok;
}
