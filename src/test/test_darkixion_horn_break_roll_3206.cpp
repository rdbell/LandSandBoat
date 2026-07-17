#include "test_darkixion_horn_break_roll_3206.h"

#include "map/darkixion_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "darkixion HornBreakRoll 3206 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline checkHornBreak 5% roll for dual-wire cross-check
// (slice 3206; residual 2907):
//   math.random(1, 100) <= 5  →  roll >= 1 && roll <= 5
auto inlineHornBreakRoll(const int32 roll1to100) -> bool
{
    return roll1to100 >= 1 && roll1to100 <= 5;
}

// Positive if/else pin matching free function / capacity body (slice 3206).
// Avoid De Morgan rewrites of the range conjunction (QF1001).
auto pinHornBreakRoll(const int32 roll1to100) -> bool
{
    if (roll1to100 >= 1)
    {
        if (roll1to100 <= darkixionhelpers::HornBreakChancePercent)
        {
            return true;
        }
    }
    return false;
}

} // namespace

// Pure dual-wire expansion for darkixionhelpers::HornBreakRoll
// (Lua checkHornBreak 5% roll after CanBreakHorn; OmegaXI internal/darkixion;
// dedicated slice 3206).
//
// Coverage:
//   - rolls 1..5 → break true
//   - poles 0, 6, 100, -1 → break false
//   - free == inline == pin (positive if/else)
//   - residual 0985 / 2907 pins still hold
//   - poles 0, 1, 5, 6, 100, -1
auto runDarkixionHornBreakRoll3206SelfTests() -> bool
{
    using darkixionhelpers::CanBreakHorn;
    using darkixionhelpers::HornBreakChancePercent;
    using darkixionhelpers::HornBreakRoll;
    using darkixionhelpers::kAnimNormal;

    bool ok = true;

    ok = expect(HornBreakChancePercent == 5, "HornBreakChancePercent pin 5") && ok;

    // Residual 0985 / 2907 HornBreakRoll pins still hold under dual-wire.
    ok = expect(HornBreakRoll(1), "residual min success") && ok;
    ok = expect(HornBreakRoll(5), "residual threshold success") && ok;
    ok = expect(!HornBreakRoll(6), "residual above-threshold miss") && ok;
    ok = expect(!HornBreakRoll(0), "residual roll 0 miss") && ok;

    // --- Eligible break rolls (1..5) ---
    ok = expect(HornBreakRoll(1), "eligible min success roll 1") && ok;
    ok = expect(HornBreakRoll(HornBreakChancePercent), "eligible threshold roll 5") && ok;
    ok = expect(HornBreakRoll(2), "eligible mid success 2") && ok;
    ok = expect(HornBreakRoll(3), "eligible mid success 3") && ok;
    ok = expect(HornBreakRoll(4), "eligible mid success 4") && ok;

    // --- Blocked rolls (required poles 0, 6, 100, -1) ---
    ok = expect(!HornBreakRoll(0), "blocked roll 0") && ok;
    ok = expect(!HornBreakRoll(6), "blocked roll 6 just above") && ok;
    ok = expect(!HornBreakRoll(100), "blocked roll 100 max miss") && ok;
    ok = expect(!HornBreakRoll(-1), "blocked roll -1 negative") && ok;
    ok = expect(!HornBreakRoll(50), "blocked mid miss 50") && ok;
    ok = expect(!HornBreakRoll(101), "blocked above max 101") && ok;

    // --- Composition table: free == inline == pin (positive if/else) ---
    // Required poles: 0, 1, 5, 6, 100, -1.
    const struct
    {
        int32       roll;
        bool        want;
        const char* label;
    } cases[] = {
        // required poles
        { 0, false, "table pole roll 0" },
        { 1, true, "table pole roll 1 min success" },
        { 5, true, "table pole roll 5 threshold" },
        { 6, false, "table pole roll 6 just above" },
        { 100, false, "table pole roll 100 max miss" },
        { -1, false, "table pole roll -1 negative" },
        // mid success band
        { 2, true, "table mid success 2" },
        { 3, true, "table mid success 3" },
        { 4, true, "table mid success 4" },
        // extra miss poles
        { 50, false, "table mid miss 50" },
        { 101, false, "table above max 101" },
        { -2, false, "table negative -2" },
    };

    for (const auto& c : cases)
    {
        const bool got     = HornBreakRoll(c.roll);
        const bool inlineC = inlineHornBreakRoll(c.roll);
        const bool pinGot  = pinHornBreakRoll(c.roll);
        const bool compose = c.roll >= 1 && c.roll <= HornBreakChancePercent;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin (positive if/else)") && ok;
        ok = expect(got == compose, "formula free==roll>=1&&roll<=5") && ok;
    }

    // Free == pin across residual poles (positive if/else pin form).
    ok = expect(HornBreakRoll(1) == pinHornBreakRoll(1), "free == pin residual min") && ok;
    ok = expect(HornBreakRoll(5) == pinHornBreakRoll(5), "free == pin residual threshold") && ok;
    ok = expect(HornBreakRoll(6) == pinHornBreakRoll(6), "free == pin residual above") && ok;
    ok = expect(HornBreakRoll(0) == pinHornBreakRoll(0), "free == pin residual roll 0") && ok;
    ok = expect(HornBreakRoll(100) == pinHornBreakRoll(100), "free == pin residual roll 100") && ok;
    ok = expect(HornBreakRoll(-1) == pinHornBreakRoll(-1), "free == pin residual negative") && ok;

    // Dense compose identity over inject range and neighbors.
    for (int32 roll = -2; roll <= 101; ++roll)
    {
        const bool got  = HornBreakRoll(roll);
        const bool want = roll >= 1 && roll <= HornBreakChancePercent;
        ok = expect(got == want, "compose range free == formula") && ok;
        ok = expect(got == inlineHornBreakRoll(roll), "compose range free == inline") && ok;
        ok = expect(got == pinHornBreakRoll(roll), "compose range free == pin positive if/else") && ok;
    }

    // Production checkHornBreak path semantics:
    // Eligible CanBreakHorn → host may apply HornBreakRoll then changeHornState.
    // Blocked gate → no roll / no state change.
    ok = expect(CanBreakHorn(false, kAnimNormal, true), "compose CanBreakHorn setup") && ok;
    ok = expect(HornBreakRoll(HornBreakChancePercent), "compose eligible + threshold") && ok;
    ok = expect(!HornBreakRoll(HornBreakChancePercent + 1), "compose eligible + above") && ok;
    ok = expect(!CanBreakHorn(true, kAnimNormal, true), "compose busy gate blocks roll path") && ok;

    return ok;
}
