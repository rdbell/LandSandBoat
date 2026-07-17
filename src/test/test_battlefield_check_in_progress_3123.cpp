#include "test_battlefield_check_in_progress_3123.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldCheckInProgress 3123 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline onTick CheckInProgress gate formula for dual-wire cross-check
// (slice 3123):
//   !attacked
auto inlineShouldCheckInProgress(const bool attacked) -> bool
{
    return !attacked;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldCheckInProgress
// (!attacked on onTick CheckInProgress path; slice 3123).
// Dense 2¹ poles: attacked false/true; free == inline == !attacked.
auto runBattlefieldCheckInProgress3123SelfTests() -> bool
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
    ok = expect(ShouldCheckInProgress(false), "residual: !attacked checks in progress") && ok;

    const struct
    {
        bool        attacked;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual-wire poles.
        { false, true, "attacked=false → check in progress" },
        { true, false, "attacked=true → skip CheckInProgress" },

        // Residual 1361 pin.
        { false, true, "residual ShouldCheckInProgress(false)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldCheckInProgress(c.attacked);
        const bool inlineF = inlineShouldCheckInProgress(c.attacked);
        const bool wantPin = !c.attacked;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldCheckInProgress dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldCheckInProgress == pin formula !attacked") && ok;
    }

    // Pin composition: check iff !attacked.
    ok = expect(ShouldCheckInProgress(false), "!attacked must check in progress") && ok;
    ok = expect(!ShouldCheckInProgress(true), "attacked must not check in progress") && ok;

    // Dense compose over 2¹ poles: free == inline == pin.
    for (const bool attacked : { false, true })
    {
        const bool got  = ShouldCheckInProgress(attacked);
        const bool want = !attacked;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldCheckInProgress(attacked), "compose free == inline") && ok;
    }

    // Explicit polarity.
    ok = expect(ShouldCheckInProgress(false), "polarity: !attacked must check") && ok;
    ok = expect(!ShouldCheckInProgress(true), "polarity: attacked must not check") && ok;

    // Host-style compose: onTick injects m_Attacked.
    const struct
    {
        bool        attacked;
        bool        want;
        const char* label;
    } hostCases[] = {
        { false, true, "not attacked → CheckInProgress path" },
        { true, false, "already attacked → skip CheckInProgress" },
    };

    for (const auto& c : hostCases)
    {
        const bool got = ShouldCheckInProgress(c.attacked);
        ok             = expect(got == c.want, c.label) && ok;
        ok             = expect(got == inlineShouldCheckInProgress(c.attacked), "host free == inline") && ok;
        ok             = expect(got == !c.attacked, "host free == pin !attacked") && ok;
    }

    // Production onTick path semantics.
    ok = expect(ShouldCheckInProgress(false), "onTick not attacked → CheckInProgress path") && ok;
    ok = expect(!ShouldCheckInProgress(true), "onTick attacked → not CheckInProgress path") && ok;

    // Sibling residual tick halves (1361) remain independent.
    ok = expect(ShouldAdvanceBattlefieldTick(true) && !ShouldAdvanceBattlefieldTick(false),
                "sibling residual ShouldAdvanceBattlefieldTick still holds") &&
         ok;
    ok = expect(ShouldHoldFightTick(StatusLocked) && !ShouldHoldFightTick(StatusOpen),
                "sibling residual ShouldHoldFightTick still holds") &&
         ok;
    ok = expect(ShouldCaptureFinishTime(StatusWon) && !ShouldCaptureFinishTime(StatusLocked),
                "sibling residual ShouldCaptureFinishTime still holds") &&
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

    // Explicit dual-wire poles: free == inline == !attacked for dense 2¹.
    for (const bool attacked : { false, true })
    {
        const bool got = ShouldCheckInProgress(attacked);
        ok             = expect(got == !attacked, "host inject dual-wire identity") && ok;
        ok             = expect(got == inlineShouldCheckInProgress(attacked), "host inject free == inline") && ok;
    }

    // Residual independence: check-in-progress gate distinct from insert gates.
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

    // Sibling dual-wires 3059 / 3087 / 3102 left alone: residual pins still hold.
    ok = expect(ShouldApplyLevelCap(75) && !ShouldApplyLevelCap(0),
                "sibling ShouldApplyLevelCap residual still holds (3059 left alone)") &&
         ok;
    ok = expect(ShouldAddSjRestriction(0x00) && !ShouldAddSjRestriction(RulesAllowSubjobs),
                "sibling ShouldAddSjRestriction residual still holds (3087 left alone)") &&
         ok;
    ok = expect(ShouldClearLevelRestriction(0) && !ShouldClearLevelRestriction(75),
                "sibling ShouldClearLevelRestriction residual still holds (3102 left alone)") &&
         ok;

    return ok;
}
