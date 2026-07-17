#include "test_spell_return_zero_no_mp_cost_3402.h"

#include "map/spell_cost_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spell ShouldReturnZeroNoMPCost 3402 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CalculateSpellCost hasMPCost gate for dual-wire cross-check
// (dedicated slice 3402 expand residual 2969):
//   !hasMPCost
auto inlineShouldReturnZeroNoMPCost(const bool hasMPCost) -> bool
{
    return !hasMPCost;
}

// Compact dual-wire pin matching Go pinShouldReturnZeroNoMPCost3402 /
// C++ capacity:
//   !hasMPCost
auto pinShouldReturnZeroNoMPCost(const bool hasMPCost) -> bool
{
    return !hasMPCost;
}

} // namespace

// Pure dual-wire expansion for spellcosthelpers::ShouldReturnZeroNoMPCost
// (!hasMPCost; dedicated slice 3402 expand residual 2969 / prior dedicated
// 3183 / pure 2104).
//
// Coverage:
//   - free == inline == pin == !hasMPCost
//   - residual 2104 / 2969 pins still hold
//   - prior dedicated 3183 poles still hold
//   - both bool poles (hasMPCost false/true)
//   - sibling residual independence (null 2964 not re-expanded)
auto runSpellReturnZeroNoMPCost3402SelfTests() -> bool
{
    using spellcosthelpers::ShouldReturnZeroNoMPCost;
    using spellcosthelpers::ShouldReturnZeroNullSpell;

    bool ok = true;

    // Residual 2104 / 2969 / prior dedicated 3183 pins still hold under dual-wire.
    ok = expect(ShouldReturnZeroNoMPCost(false), "residual 2969/2104: no MP cost returns zero") && ok;
    ok = expect(!ShouldReturnZeroNoMPCost(true), "residual 2969/2104: has MP cost proceeds") && ok;
    ok = expect(ShouldReturnZeroNoMPCost(false), "prior dedicated 3183: no MP cost returns zero") && ok;
    ok = expect(!ShouldReturnZeroNoMPCost(true), "prior dedicated 3183: has MP cost proceeds") && ok;

    const struct
    {
        bool        hasMPCost;
        bool        want;
        const char* label;
    } cases[] = {
        { false, true, "pole no MP cost → return zero" },
        { true, false, "pole has MP cost → proceed" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldReturnZeroNoMPCost(c.hasMPCost);
        const bool inlineF = inlineShouldReturnZeroNoMPCost(c.hasMPCost);
        const bool pinF    = pinShouldReturnZeroNoMPCost(c.hasMPCost);
        const bool wantPin = !c.hasMPCost;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReturnZeroNoMPCost dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldReturnZeroNoMPCost dual-wire == pin formula") && ok;
        ok = expect(got == wantPin, "ShouldReturnZeroNoMPCost == pin formula !hasMPCost") && ok;
    }

    // Pin composition: negation of hasMPCost only.
    ok = expect(ShouldReturnZeroNoMPCost(false), "!hasMPCost must return zero") && ok;
    ok = expect(!ShouldReturnZeroNoMPCost(true), "hasMPCost must not return zero") && ok;

    // Explicit polarity: zero is exact ! of inject value.
    for (const bool hasMPCost : { false, true })
    {
        const bool got  = ShouldReturnZeroNoMPCost(hasMPCost);
        const bool want = !hasMPCost;
        ok              = expect(got == want, "polarity: free == !hasMPCost") && ok;
        ok              = expect(!(got && hasMPCost), "polarity: never zero when hasMPCost") && ok;
        ok              = expect(!(!got && !hasMPCost), "polarity: always zero when !hasMPCost") && ok;
    }

    // Host path: after null gate, before pure cost body.
    const struct
    {
        bool        hasMPCost;
        bool        wantZero;
        const char* label;
    } hostCases[] = {
        { false, true, "PSpell->hasMPCost() false → return zero before cost" },
        { true, false, "PSpell->hasMPCost() true → proceed to pure cost" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldReturnZeroNoMPCost(c.hasMPCost);
        const bool inlineF = inlineShouldReturnZeroNoMPCost(c.hasMPCost);
        const bool pinF    = pinShouldReturnZeroNoMPCost(c.hasMPCost);

        ok = expect(got == c.wantZero, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinF, "host compose dual-wire free == pin") && ok;
        ok = expect(got == !c.hasMPCost, "host compose free == !hasMPCost") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool hasMPCost : { false, true })
    {
        const bool got  = ShouldReturnZeroNoMPCost(hasMPCost);
        const bool want = !hasMPCost;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldReturnZeroNoMPCost(hasMPCost),
                    "compose free == inline") &&
             ok;
        ok = expect(got == pinShouldReturnZeroNoMPCost(hasMPCost), "compose free == pin") && ok;
    }

    // Residual independence (2104 / 2964 / 2969 / 3183): null zero and
    // no-MP-cost zero both block via different pure helpers. Sibling 2964
    // left alone.
    ok = expect(ShouldReturnZeroNullSpell(true) && ShouldReturnZeroNoMPCost(false),
                "null zero and no-MP-cost zero both block") &&
         ok;
    ok = expect(!ShouldReturnZeroNoMPCost(true),
                "has MP cost must proceed even if null gate zeros first") &&
         ok;
    ok = expect(!ShouldReturnZeroNullSpell(false),
                "null residual still holds under dual-wire") &&
         ok;

    return ok;
}
