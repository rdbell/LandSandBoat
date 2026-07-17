#include "test_garden_should_wilt_3525.h"

#include "map/utils/garden_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "garden should wilt 3525 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectU32(const uint32 got, const uint32 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "garden should wilt 3525 self-test failed: " << label
                  << " got=" << got << " want=" << want << '\n';
        return false;
    }
    return true;
}

// Inline UpdateGardening wilt formula for dual-wire cross-checks
// (dedicated slice 3525 expand residual 2926):
//   wiltTime = VANADAYS_TO_WILT + wiltBonus
//   true if daysSince > VANADAYS_TO_GUARANTEE_WILT + wiltTime
//   if wasExamined: false
//   else stageDuration > wiltTime || stageDuration + daysSince > wiltTime
// Positive early-return form matching production free function / capacity.
auto inlineShouldWilt3525(const uint32 stageDurationVanadays, const uint32 daysSinceStageChange,
                          const uint32 wiltBonus, const bool wasExamined) -> bool
{
    constexpr uint32 vanaDaysToWilt           = 36;
    constexpr uint32 vanaDaysToGuaranteeWilt = 144;
    const uint32     wiltTime                = vanaDaysToWilt + wiltBonus;
    if (daysSinceStageChange > vanaDaysToGuaranteeWilt + wiltTime)
    {
        return true;
    }
    if (wasExamined)
    {
        return false;
    }
    return stageDurationVanadays > wiltTime ||
           stageDurationVanadays + daysSinceStageChange > wiltTime;
}

// Compact dual-wire pin matching free function / capacity body (slice 3525).
// Positive early-return form only — avoid QF1001 De Morgan rewrites of the
// production compound OR.
auto pinShouldWilt3525(const uint32 stageDurationVanadays, const uint32 daysSinceStageChange,
                       const uint32 wiltBonus, const bool wasExamined) -> bool
{
    using gardenutilshelpers::VanaDaysToGuaranteeWilt;
    using gardenutilshelpers::VanaDaysToWilt;

    const uint32 wiltTime = VanaDaysToWilt + wiltBonus;
    if (daysSinceStageChange > VanaDaysToGuaranteeWilt + wiltTime)
    {
        return true;
    }
    if (wasExamined)
    {
        return false;
    }
    return stageDurationVanadays > wiltTime ||
           stageDurationVanadays + daysSinceStageChange > wiltTime;
}

} // namespace

// Pure dual-wire expansion for gardenutilshelpers::ShouldWilt
// (UpdateGardening wilt branch in gardenutils.cpp; OmegaXI
// internal/gardenutils; slice 3525 dedicated expand residual 2926;
// residual pure 0795; prior dedicated 3339 — formula UNCHANGED / bit-identical).
//
// Coverage:
//   - free == inline == pin (positive capacity early-return form)
//   - residual 3339 / 2926 / 0795 pins still hold
//   - pin matches capacity body (no De Morgan QF1001 rewrites)
// Residual suites remain: test_garden_should_wilt_2926 /
// runGardenShouldWilt2926SelfTests; test_garden_should_wilt_3339 /
// runGardenShouldWilt3339SelfTests.
auto runGardenShouldWilt3525SelfTests() -> bool
{
    using gardenutilshelpers::ShouldWilt;
    using gardenutilshelpers::VanaDaysToGuaranteeWilt;
    using gardenutilshelpers::VanaDaysToWilt;

    bool ok = true;

    // --- Constants pinned from Go wilt.go ---
    ok = expectU32(VanaDaysToWilt, 36, "VanaDaysToWilt") && ok;
    ok = expectU32(VanaDaysToGuaranteeWilt, 144, "VanaDaysToGuaranteeWilt") && ok;

    // Residual 0795 / 2926 / 3339 pins still hold under dedicated dual-wire.
    ok = expect(ShouldWilt(40, 0, 0, false), "residual: stageDuration > wiltTime unexamined") && ok;
    ok = expect(!ShouldWilt(40, 0, 0, true), "residual: examined blocks soft") && ok;
    ok = expect(ShouldWilt(20, 20, 0, false), "residual: sum > wiltTime unexamined") && ok;
    ok = expect(!ShouldWilt(10, 10, 0, false), "residual: sum under wiltTime") && ok;
    ok = expect(ShouldWilt(1, 181, 0, true), "residual: guarantee even if examined") && ok;
    ok = expect(!ShouldWilt(1, 180, 0, true), "residual: guarantee boundary examined") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        uint32      stageDuration;
        uint32      daysSince;
        uint32      wiltBonus;
        bool        wasExamined;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 0795 / 2926 / 3339 pins.
        { 40, 0, 0, false, true, "residual stage soft wilt" },
        { 40, 0, 0, true, false, "residual stage soft examined" },
        { 20, 20, 0, false, true, "residual sum soft wilt" },
        { 10, 10, 0, false, false, "residual sum under" },
        { 40, 0, 10, false, false, "residual bonus blocks" },
        { 47, 0, 10, false, true, "residual bonus soft wilt" },
        { 36, 0, 0, false, false, "residual equal soft boundary" },
        { 36, 1, 0, false, true, "residual equal + 1 day" },
        { 1, 181, 0, true, true, "residual guarantee examined" },
        { 1, 180, 0, true, false, "residual guarantee boundary examined" },
        { 1, 185, 4, true, true, "residual guarantee bonus" },
        { 1, 184, 4, true, false, "residual guarantee bonus boundary" },
        // Extra boundary / zero cases (match residual 2926 / 3339 table).
        { 0, 0, 0, false, false, "all zero unexamined" },
        { 0, 0, 0, true, false, "all zero examined" },
        { 37, 0, 0, false, true, "stage just over wiltTime" },
        { 0, 37, 0, false, true, "daysSince alone soft" },
        { 0, 36, 0, false, false, "daysSince alone equal no soft" },
        { 1, 181, 0, false, true, "guarantee unexamined" },
        { 100, 0, 0, true, false, "large stage examined soft blocked" },
        { 100, 200, 0, true, true, "large stage guarantee still wilts" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldWilt(c.stageDuration, c.daysSince, c.wiltBonus, c.wasExamined);
        const bool inlineGot = inlineShouldWilt3525(c.stageDuration, c.daysSince, c.wiltBonus, c.wasExamined);
        const bool pinGot    = pinShouldWilt3525(c.stageDuration, c.daysSince, c.wiltBonus, c.wasExamined);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire free == inline") && ok;
        ok                   = expect(got == pinGot, "dual-wire free == pin positive capacity form") && ok;
    }

    // Explicit residual poles free == inline == pin.
    const struct
    {
        uint32      stageDuration;
        uint32      daysSince;
        uint32      wiltBonus;
        bool        wasExamined;
        const char* label;
    } poles[] = {
        { 40, 0, 0, false, "soft stage" },
        { 40, 0, 0, true, "soft examined" },
        { 20, 20, 0, false, "soft sum" },
        { 10, 10, 0, false, "under" },
        { 1, 181, 0, true, "guarantee" },
        { 1, 180, 0, true, "guarantee boundary" },
        { 36, 0, 0, false, "equal boundary" },
        { 36, 1, 0, false, "equal + 1" },
    };
    for (const auto& pole : poles)
    {
        const bool got       = ShouldWilt(pole.stageDuration, pole.daysSince, pole.wiltBonus, pole.wasExamined);
        const bool inlineGot = inlineShouldWilt3525(pole.stageDuration, pole.daysSince, pole.wiltBonus, pole.wasExamined);
        const bool pinGot    = pinShouldWilt3525(pole.stageDuration, pole.daysSince, pole.wiltBonus, pole.wasExamined);
        ok                   = expect(got == inlineGot && got == pinGot, pole.label) && ok;
    }

    // --- Production UpdateGardening path semantics ---
    // true  → setStage(WILTED) + setStageTimestamp(vanatime + VANATIME_FOR_WILT_STAGE)
    // false → GrowToNextStage
    ok = expect(ShouldWilt(40, 0, 0, false), "UpdateGardening soft wilt path") && ok;
    ok = expect(ShouldWilt(1, 181, 0, true), "UpdateGardening guarantee wilt path") && ok;
    ok = expect(!ShouldWilt(10, 10, 0, false), "UpdateGardening grow path") && ok;
    ok = expect(!ShouldWilt(40, 0, 0, true), "UpdateGardening examined grow (no soft)") && ok;

    return ok;
}
