#include "test_party_apply_sync_disable_3025.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldApplySyncDisableToMember 3025 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::SetSyncTarget disable-path per-member filter for dual-wire
// cross-check (slice 3025):
//   isPC && notDisappear
auto inlineShouldApplySyncDisableToMember(const bool isPC, const bool notDisappear) -> bool
{
    return isPC && notDisappear;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldApplySyncDisableToMember
// (CParty::SetSyncTarget DISABLE isPC && notDisappear gate; slice 3025).
auto runPartyApplySyncDisable3025SelfTests() -> bool
{
    using partyhelpers::ClassifySetSyncTarget;
    using partyhelpers::LevelSyncDisableDurationSeconds;
    using partyhelpers::set_sync_target_gate;
    using partyhelpers::ShouldApplySyncDisableToMember;
    using partyhelpers::ShouldApplySyncEnableToMember;
    using partyhelpers::ShouldStartSyncDisableCountdown;

    bool ok = true;

    // Residual 1334 pins still hold under dual-wire.
    ok = expect(ShouldApplySyncDisableToMember(true, true), "residual apply PC visible") && ok;
    ok = expect(!ShouldApplySyncDisableToMember(false, true), "residual skip non-PC visible") && ok;
    ok = expect(!ShouldApplySyncDisableToMember(true, false), "residual skip PC disappear") && ok;

    const struct
    {
        bool        isPC;
        bool        notDisappear;
        bool        want;
        const char* label;
    } cases[] = {
        // Truth table poles — isPC × notDisappear.
        { true, true, true, "PC visible applies" },
        { true, false, false, "PC disappear skips" },
        { false, true, false, "non-PC visible skips" },
        { false, false, false, "non-PC disappear skips" },

        // Residual 1334 polarity pins.
        { true, true, true, "residual apply both true" },
        { false, true, false, "residual skip non-PC" },
        { true, false, false, "residual skip disappear" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplySyncDisableToMember(c.isPC, c.notDisappear);
        const bool inlineF = inlineShouldApplySyncDisableToMember(c.isPC, c.notDisappear);
        const bool wantPin = c.isPC && c.notDisappear;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplySyncDisableToMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldApplySyncDisableToMember == pin formula isPC && notDisappear") && ok;
    }

    // Pin composition: both gates required (zone is NOT checked on disable).
    ok = expect(ShouldApplySyncDisableToMember(true, true), "PC visible must apply") && ok;
    ok = expect(!ShouldApplySyncDisableToMember(true, false), "PC disappear must skip") && ok;
    ok = expect(!ShouldApplySyncDisableToMember(false, true), "non-PC must skip") && ok;
    ok = expect(!ShouldApplySyncDisableToMember(false, false), "all-false must skip") && ok;

    // Dense compose: full 2×2 free == inline == pin.
    for (const bool isPC : { false, true })
    {
        for (const bool notDisappear : { false, true })
        {
            const bool got  = ShouldApplySyncDisableToMember(isPC, notDisappear);
            const bool want = isPC && notDisappear;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldApplySyncDisableToMember(isPC, notDisappear),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production CParty::SetSyncTarget DISABLE path semantics ---
    // Host injects:
    //   isPC         = i->objtype == TYPE_PC
    //   notDisappear = member != nullptr && member->status != DISAPPEAR
    // when true  → optional ShouldStartSyncDisableCountdown → battle message / SetDuration
    // when false → continue (skip member)
    // Note: unlike enable-path (3015), zone is not checked on disable.
    ok = expect(ShouldApplySyncDisableToMember(true, true), "SetSyncTarget DISABLE PC visible → apply path") && ok;
    ok = expect(!ShouldApplySyncDisableToMember(true, false), "SetSyncTarget DISABLE PC disappear → skip path") && ok;
    ok = expect(!ShouldApplySyncDisableToMember(false, true), "SetSyncTarget DISABLE non-PC visible → skip path") && ok;
    ok = expect(!ShouldApplySyncDisableToMember(false, false), "SetSyncTarget DISABLE all-false → skip path") && ok;

    // Explicit dual-wire: free function is isPC && notDisappear of injects.
    for (const bool isPC : { false, true })
    {
        for (const bool notDisappear : { false, true })
        {
            const bool want = isPC && notDisappear;
            ok              = expect(ShouldApplySyncDisableToMember(isPC, notDisappear) == want,
                        "host inject identity") &&
                 ok;
            ok = expect(ShouldApplySyncDisableToMember(isPC, notDisappear) ==
                            inlineShouldApplySyncDisableToMember(isPC, notDisappear),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    // Residual sibling SetSyncTarget halves remain independent of the disable
    // filter (1334 suite coexistence; sibling enable / countdown dual-wires).
    ok = expect(ClassifySetSyncTarget(false, true, true, 50, true, false) == set_sync_target_gate::DISABLED,
                "residual ClassifySetSyncTarget disabled pin") &&
         ok;
    ok = expect(ClassifySetSyncTarget(true, true, true, 50, true, false) == set_sync_target_gate::ENABLE,
                "residual ClassifySetSyncTarget enable pin") &&
         ok;
    ok = expect(ShouldApplySyncEnableToMember(true, true, true), "residual ShouldApplySyncEnableToMember (sibling 3015)") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(true, false, true), "residual ShouldApplySyncEnableToMember disappear") && ok;
    ok = expect(ShouldStartSyncDisableCountdown(true, true), "residual ShouldStartSyncDisableCountdown (sibling 3016)") && ok;
    ok = expect(!ShouldStartSyncDisableCountdown(true, false), "residual ShouldStartSyncDisableCountdown timed") && ok;
    ok = expect(LevelSyncDisableDurationSeconds == 30, "residual LevelSyncDisableDurationSeconds") && ok;

    return ok;
}
