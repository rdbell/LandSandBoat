#include "test_darkixion_horn_break_roll_2907.h"

#include "map/darkixion_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "darkixion HornBreakRoll 2907 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline checkHornBreak 5% roll for dual-wire cross-check:
//   math.random(1, 100) <= 5  →  roll >= 1 && roll <= 5
auto inlineHornBreakRoll(const int32 roll1to100) -> bool
{
    return roll1to100 >= 1 && roll1to100 <= 5;
}

} // namespace

// Pure dual-wire expansion for darkixionhelpers::HornBreakRoll
// (Lua checkHornBreak 5% roll after CanBreakHorn gate; slice 2907).
auto runDarkixionHornBreakRoll2907SelfTests() -> bool
{
    using darkixionhelpers::CanBreakHorn;
    using darkixionhelpers::HornBreakChancePercent;
    using darkixionhelpers::HornBreakRoll;
    using darkixionhelpers::kAnimNormal;

    bool ok = true;

    ok = expect(HornBreakChancePercent == 5, "HornBreakChancePercent pin 5") && ok;

    // Residual 0985 HornBreakRoll pins.
    ok = expect(HornBreakRoll(1), "residual min success") && ok;
    ok = expect(HornBreakRoll(5), "residual threshold success") && ok;
    ok = expect(!HornBreakRoll(6), "residual above-threshold miss") && ok;
    ok = expect(!HornBreakRoll(0), "residual roll 0 miss") && ok;

    // Dual-wire matches inline formula across a table.
    const struct
    {
        int32       roll;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, true, "table min success" },
        { 5, true, "table threshold success" },
        { 2, true, "table mid success 2" },
        { 3, true, "table mid success 3" },
        { 4, true, "table mid success 4" },
        { 6, false, "table just above threshold" },
        { 50, false, "table mid miss" },
        { 100, false, "table max miss" },
        { 0, false, "table roll 0 out of range" },
        { -1, false, "table negative roll" },
        { 101, false, "table above max range" },
        { 5, true, "table residual threshold pin" },
        { 1, true, "table residual min pin" },
    };

    for (const auto& c : cases)
    {
        const bool got       = HornBreakRoll(c.roll);
        const bool inlineGot = inlineHornBreakRoll(c.roll);
        const bool wantPin   = c.roll >= 1 && c.roll <= HornBreakChancePercent;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == wantPin, "dual-wire free == pin formula") && ok;
    }

    // Dense compose range identity: rolls -2..101.
    for (int32 roll = -2; roll <= 101; ++roll)
    {
        const bool got  = HornBreakRoll(roll);
        const bool want = roll >= 1 && roll <= HornBreakChancePercent;
        ok = expect(got == want, "compose range free == formula") && ok;
        ok = expect(got == inlineHornBreakRoll(roll), "compose range free == inline") && ok;
    }

    // Host compose: CanBreakHorn gate then HornBreakRoll (RNG / changeHornState
    // still host-owned). Eligible gate + threshold succeeds; above fails.
    ok = expect(CanBreakHorn(false, kAnimNormal, true), "compose CanBreakHorn setup") && ok;
    ok = expect(HornBreakRoll(HornBreakChancePercent), "compose eligible + threshold") && ok;
    ok = expect(!HornBreakRoll(HornBreakChancePercent + 1), "compose eligible + above") && ok;

    return ok;
}
