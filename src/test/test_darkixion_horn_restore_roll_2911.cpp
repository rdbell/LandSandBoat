#include "test_darkixion_horn_restore_roll_2911.h"

#include "map/darkixion_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "darkixion HornRestoreRoll 2911 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Damsel Memento 25% roll for dual-wire cross-check:
//   math.random(1, 100) <= 25  →  roll >= 1 && roll <= 25
auto inlineHornRestoreRoll(const int32 roll1to100) -> bool
{
    return roll1to100 >= 1 && roll1to100 <= 25;
}

} // namespace

// Pure dual-wire expansion for darkixionhelpers::HornRestoreRoll
// (Lua Damsel Memento 25% roll after CanRestoreHorn gate; slice 2911).
auto runDarkixionHornRestoreRoll2911SelfTests() -> bool
{
    using darkixionhelpers::CanRestoreHorn;
    using darkixionhelpers::HornRestoreChancePercent;
    using darkixionhelpers::HornRestoreRoll;
    using darkixionhelpers::kAnimHornBroken;

    bool ok = true;

    ok = expect(HornRestoreChancePercent == 25, "HornRestoreChancePercent pin 25") && ok;

    // Residual 0985 HornRestoreRoll pins.
    ok = expect(HornRestoreRoll(1), "residual min success") && ok;
    ok = expect(HornRestoreRoll(25), "residual threshold success") && ok;
    ok = expect(!HornRestoreRoll(26), "residual above-threshold miss") && ok;
    ok = expect(!HornRestoreRoll(0), "residual roll 0 miss") && ok;

    // Dual-wire matches inline formula across a table.
    const struct
    {
        int32       roll;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, true, "table min success" },
        { 25, true, "table threshold success" },
        { 12, true, "table mid success 12" },
        { 13, true, "table mid success 13" },
        { 24, true, "table near threshold 24" },
        { 26, false, "table just above threshold" },
        { 50, false, "table mid miss" },
        { 100, false, "table max miss" },
        { 0, false, "table roll 0 out of range" },
        { -1, false, "table negative roll" },
        { 101, false, "table above max range" },
        { 25, true, "table residual threshold pin" },
        { 1, true, "table residual min pin" },
    };

    for (const auto& c : cases)
    {
        const bool got       = HornRestoreRoll(c.roll);
        const bool inlineGot = inlineHornRestoreRoll(c.roll);
        const bool wantPin   = c.roll >= 1 && c.roll <= HornRestoreChancePercent;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == wantPin, "dual-wire free == pin formula") && ok;
    }

    // Dense compose range identity: rolls -2..101.
    for (int32 roll = -2; roll <= 101; ++roll)
    {
        const bool got  = HornRestoreRoll(roll);
        const bool want = roll >= 1 && roll <= HornRestoreChancePercent;
        ok = expect(got == want, "compose range free == formula") && ok;
        ok = expect(got == inlineHornRestoreRoll(roll), "compose range free == inline") && ok;
    }

    // Host compose: CanRestoreHorn gate then HornRestoreRoll (RNG / stun /
    // changeHornState still host-owned). Eligible gate + threshold succeeds;
    // above fails.
    ok = expect(CanRestoreHorn(kAnimHornBroken), "compose CanRestoreHorn setup") && ok;
    ok = expect(HornRestoreRoll(HornRestoreChancePercent), "compose eligible + threshold") && ok;
    ok = expect(!HornRestoreRoll(HornRestoreChancePercent + 1), "compose eligible + above") && ok;

    return ok;
}
