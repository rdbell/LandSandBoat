#include "test_party_push_effects_packet_3041.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldPushEffectsPacket 3041 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::PushEffectsPacket m_EffectsChanged gate for dual-wire
// cross-check (slice 3041):
//   effectsChanged
auto inlineShouldPushEffectsPacket(const bool effectsChanged) -> bool
{
    return effectsChanged;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldPushEffectsPacket
// (CParty::PushEffectsPacket m_EffectsChanged gate; slice 3041).
auto runPartyPushEffectsPacket3041SelfTests() -> bool
{
    using partyhelpers::ShouldIncludeInGroupEffects;
    using partyhelpers::ShouldPushEffectsPacket;
    using partyhelpers::ShouldPushPartyPacketToMember;

    bool ok = true;

    // Residual 1336 pins still hold under dual-wire.
    ok = expect(ShouldPushEffectsPacket(true), "residual effects changed true") && ok;
    ok = expect(!ShouldPushEffectsPacket(false), "residual effects changed false") && ok;

    const struct
    {
        bool        effectsChanged;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, false, "unchanged early return" },
        { true, true, "changed continues body" },

        // Residual 1336 polarity pins.
        { false, false, "residual ShouldPushEffectsPacket(false)" },
        { true, true, "residual ShouldPushEffectsPacket(true)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldPushEffectsPacket(c.effectsChanged);
        const bool inlineF = inlineShouldPushEffectsPacket(c.effectsChanged);
        const bool wantPin = c.effectsChanged;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldPushEffectsPacket dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldPushEffectsPacket == pin formula effectsChanged") && ok;
    }

    // Pin composition: identity only.
    ok = expect(!ShouldPushEffectsPacket(false), "false must early-return (no work)") && ok;
    ok = expect(ShouldPushEffectsPacket(true), "true must continue PushEffectsPacket body") && ok;

    // Dense compose: full 2^1 boolean space.
    // free == inline identity == pin formula effectsChanged.
    for (const bool effectsChanged : { false, true })
    {
        const bool got  = ShouldPushEffectsPacket(effectsChanged);
        const bool want = effectsChanged;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldPushEffectsPacket(effectsChanged),
                    "compose free == inline") &&
             ok;
    }

    // --- Production CParty::PushEffectsPacket path semantics ---
    // Host injects:
    //   effectsChanged = m_EffectsChanged  // set true by EffectsChanged()
    // when true  → GetPartyInfo / sameZoneMembers / GROUP_EFFECTS / clear flag
    // when false → return early (no packet work)
    // Host GetPartyInfo / ShouldIncludeInGroupEffects loop / packet sit outside.
    ok = expect(!ShouldPushEffectsPacket(false), "PushEffects unchanged → early return path") && ok;
    ok = expect(ShouldPushEffectsPacket(true), "PushEffects changed → continue body path") && ok;

    // Explicit dual-wire: free == identity inline == pin for host poles.
    const struct
    {
        bool        effectsChanged;
        const char* label;
    } hostPoles[] = {
        { false, "early return path" },
        { true, "continue body path" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldPushEffectsPacket(p.effectsChanged);
        const bool inlineF = inlineShouldPushEffectsPacket(p.effectsChanged);
        const bool want    = p.effectsChanged;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Sibling dual-wire group-effects filter remains independent of the
    // m_EffectsChanged gate (1336 residual / 3036 dual-wire coexistence).
    ok = expect(ShouldIncludeInGroupEffects(10, 10, 2, 1, true, true),
                "residual ShouldIncludeInGroupEffects include") &&
         ok;
    ok = expect(!ShouldIncludeInGroupEffects(11, 10, 2, 1, true, true),
                "residual ShouldIncludeInGroupEffects other party") &&
         ok;

    // Sibling dual-wire PushPacket filter remains independent
    // (1335 residual / 3031 dual-wire coexistence).
    ok = expect(ShouldPushPartyPacketToMember(true, 2, 1, true, false, 0, 230),
                "residual ShouldPushPartyPacketToMember all-zones") &&
         ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 1, 1, true, false, 0, 230),
                "residual ShouldPushPartyPacketToMember sender") &&
         ok;

    // EffectsChanged host path is orthogonal: sets m_EffectsChanged = true
    // before PushEffectsPacket; free function remains identity on inject.
    // Simulates: EffectsChanged() then PushEffectsPacket gate.
    bool flag = false;
    ok        = expect(!ShouldPushEffectsPacket(flag), "before EffectsChanged → no push") && ok;
    flag      = true; // EffectsChanged()
    ok        = expect(ShouldPushEffectsPacket(flag), "after EffectsChanged → push path") && ok;
    // Host clears flag after body; next gate is false again.
    flag = false; // m_EffectsChanged = false at end of PushEffectsPacket
    ok   = expect(!ShouldPushEffectsPacket(flag),
                "after clear → no push until next EffectsChanged") &&
         ok;

    return ok;
}
