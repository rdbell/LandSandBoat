#include "test_garden_should_wilt_2926.h"

#include "map/utils/garden_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "garden should wilt 2926 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectU32(const uint32 got, const uint32 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "garden should wilt 2926 self-test failed: " << label
                  << " got=" << got << " want=" << want << '\n';
        return false;
    }
    return true;
}

// Inline production UpdateGardening wilt formula for dual-wire cross-checks:
//   wiltTime = VANADAYS_TO_WILT + wiltBonus
//   true if daysSince > VANADAYS_TO_GUARANTEE_WILT + wiltTime
//   if wasExamined: false
//   else stageDuration > wiltTime || stageDuration + daysSince > wiltTime
auto inlineShouldWilt(const uint32 stageDurationVanadays, const uint32 daysSinceStageChange,
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

} // namespace

// Pure dual-wire expansion for gardenutilshelpers::ShouldWilt
// (UpdateGardening wilt branch in gardenutils.cpp).
auto runGardenShouldWilt2926SelfTests() -> bool
{
    using gardenutilshelpers::ShouldWilt;
    using gardenutilshelpers::VanaDaysToGuaranteeWilt;
    using gardenutilshelpers::VanaDaysToWilt;

    bool ok = true;

    // --- Constants pinned from Go wilt.go ---
    ok = expectU32(VanaDaysToWilt, 36, "VanaDaysToWilt") && ok;
    ok = expectU32(VanaDaysToGuaranteeWilt, 144, "VanaDaysToGuaranteeWilt") && ok;

    // --- Soft wilt: stageDuration exceeds wiltTime (unexamined) ---
    ok = expect(ShouldWilt(40, 0, 0, false), "stageDuration > wiltTime unexamined") && ok;
    ok = expect(!ShouldWilt(40, 0, 0, true), "stageDuration > wiltTime examined blocks soft") && ok;

    // --- Soft wilt: stageDuration + daysSince exceeds wiltTime ---
    ok = expect(ShouldWilt(20, 20, 0, false), "sum > wiltTime unexamined") && ok;
    ok = expect(!ShouldWilt(10, 10, 0, false), "sum 20 < wiltTime 36") && ok;

    // --- Wilt bonus raises threshold ---
    ok = expect(!ShouldWilt(40, 0, 10, false), "stage 40 wiltBonus 10 (wiltTime 46)") && ok;
    ok = expect(ShouldWilt(47, 0, 10, false), "stage 47 wiltBonus 10") && ok;

    // --- Soft exact boundary: stageDuration == wiltTime alone is not enough ---
    ok = expect(!ShouldWilt(36, 0, 0, false), "stageDuration == wiltTime days 0") && ok;
    ok = expect(ShouldWilt(36, 1, 0, false), "stageDuration + days == wiltTime + 1") && ok;

    // --- Guarantee wilt: daysSince > 144 + wiltTime even if examined ---
    ok = expect(ShouldWilt(1, 181, 0, true), "guarantee 181 > 180 examined") && ok;
    ok = expect(!ShouldWilt(1, 180, 0, true), "guarantee boundary 180 examined no wilt") && ok;
    ok = expect(ShouldWilt(1, 185, 4, true), "guarantee wiltBonus 4 (185 > 184)") && ok;
    ok = expect(!ShouldWilt(1, 184, 4, true), "guarantee boundary with bonus") && ok;

    // --- Composition table: dual-wire pure gate from host-injected scalars ---
    const struct
    {
        uint32      stageDuration;
        uint32      daysSince;
        uint32      wiltBonus;
        bool        wasExamined;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 0795 / wilt_test pins.
        { 40, 0, 0, false, true, "table stage soft wilt" },
        { 40, 0, 0, true, false, "table stage soft examined" },
        { 20, 20, 0, false, true, "table sum soft wilt" },
        { 10, 10, 0, false, false, "table sum under" },
        { 40, 0, 10, false, false, "table bonus blocks" },
        { 47, 0, 10, false, true, "table bonus soft wilt" },
        { 36, 0, 0, false, false, "table equal soft boundary" },
        { 36, 1, 0, false, true, "table equal + 1 day" },
        { 1, 181, 0, true, true, "table guarantee examined" },
        { 1, 180, 0, true, false, "table guarantee boundary examined" },
        { 1, 185, 4, true, true, "table guarantee bonus" },
        { 1, 184, 4, true, false, "table guarantee bonus boundary" },
        // Extra boundary / zero cases.
        { 0, 0, 0, false, false, "table all zero unexamined" },
        { 0, 0, 0, true, false, "table all zero examined" },
        { 37, 0, 0, false, true, "table stage just over wiltTime" },
        { 0, 37, 0, false, true, "table daysSince alone soft (0+37 > 36)" },
        { 0, 36, 0, false, false, "table daysSince alone equal no soft" },
        { 1, 181, 0, false, true, "table guarantee unexamined" },
        { 100, 0, 0, true, false, "table large stage examined soft blocked" },
        { 100, 200, 0, true, true, "table large stage guarantee still wilts" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldWilt(c.stageDuration, c.daysSince, c.wiltBonus, c.wasExamined);
        const bool inlineGot = inlineShouldWilt(c.stageDuration, c.daysSince, c.wiltBonus, c.wasExamined);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
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
