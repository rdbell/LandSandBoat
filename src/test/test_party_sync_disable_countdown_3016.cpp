#include "test_party_sync_disable_countdown_3016.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldStartSyncDisableCountdown 3016 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline disable/remove countdown gate for dual-wire cross-check (slice 3016):
//   hasLevelSync && durationIsZero
auto inlineShouldStartSyncDisableCountdown(const bool hasLevelSync, const bool durationIsZero) -> bool
{
    return hasLevelSync && durationIsZero;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldStartSyncDisableCountdown
// (LevelSync disable/remove hasLevelSync && durationIsZero gate; slice 3016).
auto runPartySyncDisableCountdown3016SelfTests() -> bool
{
    using partyhelpers::LevelSyncDisableDurationSeconds;
    using partyhelpers::ShouldApplySyncDisableToMember;
    using partyhelpers::ShouldApplySyncEnableToMember;
    using partyhelpers::ShouldStartSyncDisableCountdown;

    bool ok = true;

    // Residual 1334 pins still hold under dual-wire.
    ok = expect(ShouldStartSyncDisableCountdown(true, true), "residual countdown infinite sync") && ok;
    ok = expect(!ShouldStartSyncDisableCountdown(true, false), "residual no countdown timed sync") && ok;
    ok = expect(!ShouldStartSyncDisableCountdown(false, true), "residual no countdown missing effect") && ok;

    const struct
    {
        bool        hasLevelSync;
        bool        durationIsZero;
        bool        want;
        const char* label;
    } cases[] = {
        // Truth table poles — hasLevelSync × durationIsZero.
        { true, true, true, "has infinite LevelSync starts countdown" },
        { true, false, false, "has timed LevelSync skips countdown" },
        { false, true, false, "missing LevelSync skips (duration flag ignored)" },
        { false, false, false, "missing LevelSync timed pole skips" },

        // Residual 1334 polarity pins.
        { true, true, true, "residual countdown both true" },
        { true, false, false, "residual no countdown timed" },
        { false, true, false, "residual no countdown missing" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldStartSyncDisableCountdown(c.hasLevelSync, c.durationIsZero);
        const bool inlineF = inlineShouldStartSyncDisableCountdown(c.hasLevelSync, c.durationIsZero);
        const bool wantPin = c.hasLevelSync && c.durationIsZero;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldStartSyncDisableCountdown dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldStartSyncDisableCountdown == pin formula hasLevelSync && durationIsZero") && ok;
    }

    // Pin composition: both LevelSync present and infinite duration required.
    ok = expect(ShouldStartSyncDisableCountdown(true, true), "infinite LevelSync must start countdown") && ok;
    ok = expect(!ShouldStartSyncDisableCountdown(true, false), "timed LevelSync must skip countdown") && ok;
    ok = expect(!ShouldStartSyncDisableCountdown(false, true), "missing LevelSync must skip countdown") && ok;
    ok = expect(!ShouldStartSyncDisableCountdown(false, false), "missing LevelSync timed pole must skip countdown") && ok;

    // Dense compose: full 2×2 free == inline == pin.
    for (const bool hasLevelSync : { false, true })
    {
        for (const bool durationIsZero : { false, true })
        {
            const bool got  = ShouldStartSyncDisableCountdown(hasLevelSync, durationIsZero);
            const bool want = hasLevelSync && durationIsZero;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldStartSyncDisableCountdown(hasLevelSync, durationIsZero),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production countdown path semantics ---
    // Host injects (SetSyncTarget DISABLE / RemoveMember / Disband):
    //   hasLevelSync   = sync != nullptr
    //   durationIsZero = sync != nullptr && sync->GetDuration() == 0s
    // when true  → push BATTLE_MESSAGE(…, LevelSyncDisableDurationSeconds, msg),
    //              SetStartTime(now), SetDuration(LevelSyncDisableDurationSeconds)
    // when false → leave effect alone
    ok = expect(ShouldStartSyncDisableCountdown(true, true), "host infinite sync → countdown path") && ok;
    ok = expect(!ShouldStartSyncDisableCountdown(true, false), "host timed sync → skip countdown path") && ok;
    ok = expect(!ShouldStartSyncDisableCountdown(false, true), "host missing effect → skip countdown path") && ok;
    ok = expect(!ShouldStartSyncDisableCountdown(false, false), "host missing timed pole → skip countdown path") && ok;

    // Explicit dual-wire: free function is hasLevelSync && durationIsZero of injects.
    for (const bool hasLevelSync : { false, true })
    {
        for (const bool durationIsZero : { false, true })
        {
            const bool want = hasLevelSync && durationIsZero;
            ok              = expect(ShouldStartSyncDisableCountdown(hasLevelSync, durationIsZero) == want,
                        "host inject identity") &&
                 ok;
            ok = expect(ShouldStartSyncDisableCountdown(hasLevelSync, durationIsZero) ==
                            inlineShouldStartSyncDisableCountdown(hasLevelSync, durationIsZero),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    // Residual sibling SetSyncTarget halves remain independent of the countdown
    // gate (1334 suite coexistence; sibling enable filter).
    ok = expect(LevelSyncDisableDurationSeconds == 30, "residual LevelSyncDisableDurationSeconds") && ok;
    ok = expect(ShouldApplySyncDisableToMember(true, true), "residual ShouldApplySyncDisableToMember PC ok") && ok;
    ok = expect(!ShouldApplySyncDisableToMember(false, true), "residual ShouldApplySyncDisableToMember non-PC") && ok;
    ok = expect(ShouldApplySyncEnableToMember(true, true, true), "residual ShouldApplySyncEnableToMember (sibling enable)") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(true, false, true), "residual ShouldApplySyncEnableToMember disappear") && ok;

    return ok;
}
