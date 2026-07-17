#include "test_battlefield_advance_tick_3140.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldAdvanceBattlefieldTick 3140 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline onTick advance-tick gate formula for dual-wire cross-check
// (slice 3140):
//   pastTickPlusOneSecond
auto inlineShouldAdvanceBattlefieldTick(const bool pastTickPlusOneSecond) -> bool
{
    return pastTickPlusOneSecond;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldAdvanceBattlefieldTick
// (pastTickPlusOneSecond identity on onTick advance path; slice 3140).
// Dense 2¹ poles: pastTickPlusOneSecond false/true; free == inline == pin.
auto runBattlefieldAdvanceTick3140SelfTests() -> bool
{
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldAddSjRestriction;
    using battlefieldhelpers::ShouldAdvanceBattlefieldTick;
    using battlefieldhelpers::ShouldApplyLevelCap;
    using battlefieldhelpers::ShouldCaptureFinishTime;
    using battlefieldhelpers::ShouldCheckInProgress;
    using battlefieldhelpers::ShouldClearLevelRestriction;
    using battlefieldhelpers::ShouldEnterPC;
    using battlefieldhelpers::ShouldHoldFightTick;
    using battlefieldhelpers::ShouldRegisterPC;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;
    using battlefieldhelpers::RulesAllowSubjobs;
    using battlefieldhelpers::StatusLocked;
    using battlefieldhelpers::StatusOpen;
    using battlefieldhelpers::StatusWon;

    bool ok = true;

    // Residual 1361 pin still holds under dual-wire.
    ok = expect(ShouldAdvanceBattlefieldTick(true), "residual: past tick+1s advances battlefield tick") && ok;

    const struct
    {
        bool        past;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual-wire poles.
        { true, true, "pastTickPlusOneSecond=true → advance tick" },
        { false, false, "pastTickPlusOneSecond=false → skip advance" },

        // Residual 1361 pin.
        { true, true, "residual ShouldAdvanceBattlefieldTick(true)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAdvanceBattlefieldTick(c.past);
        const bool inlineF = inlineShouldAdvanceBattlefieldTick(c.past);
        const bool wantPin = c.past;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAdvanceBattlefieldTick dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAdvanceBattlefieldTick == pin formula pastTickPlusOneSecond") && ok;
    }

    // Pin composition: advance iff pastTickPlusOneSecond.
    ok = expect(ShouldAdvanceBattlefieldTick(true), "past tick+1s must advance battlefield tick") && ok;
    ok = expect(!ShouldAdvanceBattlefieldTick(false), "not past tick+1s must not advance battlefield tick") && ok;

    // Dense compose over 2¹ poles: free == inline == pin.
    for (const bool past : { false, true })
    {
        const bool got  = ShouldAdvanceBattlefieldTick(past);
        const bool want = past;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldAdvanceBattlefieldTick(past), "compose free == inline") && ok;
    }

    // Explicit polarity.
    ok = expect(ShouldAdvanceBattlefieldTick(true), "polarity: past tick+1s must advance") && ok;
    ok = expect(!ShouldAdvanceBattlefieldTick(false), "polarity: not past tick+1s must not advance") && ok;

    // Host-style compose: onTick injects time > m_Tick + 1s.
    const struct
    {
        bool        past;
        bool        want;
        const char* label;
    } hostCases[] = {
        { true, true, "past tick+1s → advance path" },
        { false, false, "not past tick+1s → skip advance" },
    };

    for (const auto& c : hostCases)
    {
        const bool got = ShouldAdvanceBattlefieldTick(c.past);
        ok             = expect(got == c.want, c.label) && ok;
        ok             = expect(got == inlineShouldAdvanceBattlefieldTick(c.past), "host free == inline") && ok;
        ok             = expect(got == c.past, "host free == pin pastTickPlusOneSecond") && ok;
    }

    // Production onTick path semantics.
    ok = expect(ShouldAdvanceBattlefieldTick(true), "onTick past tick → advance path") && ok;
    ok = expect(!ShouldAdvanceBattlefieldTick(false), "onTick not past tick → not advance path") && ok;

    // Sibling residual tick halves (1361; ShouldHoldFightTick not dual-wired).
    ok = expect(ShouldHoldFightTick(StatusLocked) && !ShouldHoldFightTick(StatusOpen),
                "sibling residual ShouldHoldFightTick still holds (not dual-wired)") &&
         ok;
    ok = expect(ShouldCaptureFinishTime(StatusWon) && !ShouldCaptureFinishTime(StatusLocked),
                "sibling residual ShouldCaptureFinishTime still holds") &&
         ok;

    // Sibling dual-wire ShouldCheckInProgress (3123; left alone).
    ok = expect(ShouldCheckInProgress(false) && !ShouldCheckInProgress(true),
                "sibling dual-wire ShouldCheckInProgress still holds (3123 left alone)") &&
         ok;

    // onTick compose: CheckInProgress gate orthogonal to advance-tick.
    ok = expect(ShouldCheckInProgress(false) && ShouldAdvanceBattlefieldTick(true),
                "compose: !attacked + past tick both gates") &&
         ok;
    ok = expect(!ShouldCheckInProgress(true) && ShouldAdvanceBattlefieldTick(true),
                "compose: attacked + past tick only advance gate") &&
         ok;
    ok = expect(ShouldCheckInProgress(false) && !ShouldAdvanceBattlefieldTick(false),
                "compose: !attacked + not past tick only check gate") &&
         ok;
    ok = expect(!ShouldCheckInProgress(true) && !ShouldAdvanceBattlefieldTick(false),
                "compose: attacked + not past tick neither gate") &&
         ok;

    // Advance + hold-fight compose.
    ok = expect(ShouldAdvanceBattlefieldTick(true) && ShouldHoldFightTick(StatusLocked),
                "compose: past tick + locked advance and hold fight tick") &&
         ok;
    ok = expect(ShouldAdvanceBattlefieldTick(true) && !ShouldHoldFightTick(StatusOpen),
                "compose: past tick + open advance without hold") &&
         ok;
    ok = expect(!ShouldAdvanceBattlefieldTick(false) && ShouldHoldFightTick(StatusLocked),
                "compose: not past tick + locked no advance, hold still true") &&
         ok;

    // Explicit dual-wire poles: free == inline == pastTickPlusOneSecond for dense 2¹.
    for (const bool past : { false, true })
    {
        const bool got = ShouldAdvanceBattlefieldTick(past);
        ok             = expect(got == past, "host inject dual-wire identity") && ok;
        ok             = expect(got == inlineShouldAdvanceBattlefieldTick(past), "host inject free == inline") && ok;
    }

    // Residual independence: advance-tick gate distinct from insert gates.
    ok = expect(ShouldRejectNullInsert(true) && !ShouldRejectNullInsert(false),
                "null-insert residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldRejectAlreadyInBattlefield(true) && !ShouldRejectAlreadyInBattlefield(false),
                "already-in residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6) && !ShouldAcceptPCUnderCapacity(6, 6),
                "PC under-capacity residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldEnterPC(true) && !ShouldEnterPC(false),
                "enter residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldRegisterPC(false, false) && !ShouldRegisterPC(false, true),
                "register residual still holds under dual-wire") &&
         ok;

    // Sibling dual-wires 3059 / 3087 / 3102 / 3123 left alone: residual pins still hold.
    ok = expect(ShouldApplyLevelCap(75) && !ShouldApplyLevelCap(0),
                "sibling ShouldApplyLevelCap residual still holds (3059 left alone)") &&
         ok;
    ok = expect(ShouldAddSjRestriction(0x00) && !ShouldAddSjRestriction(RulesAllowSubjobs),
                "sibling ShouldAddSjRestriction residual still holds (3087 left alone)") &&
         ok;
    ok = expect(ShouldClearLevelRestriction(0) && !ShouldClearLevelRestriction(75),
                "sibling ShouldClearLevelRestriction residual still holds (3102 left alone)") &&
         ok;
    ok = expect(ShouldCheckInProgress(false) && !ShouldCheckInProgress(true),
                "sibling ShouldCheckInProgress residual still holds (3123 left alone)") &&
         ok;

    return ok;
}
