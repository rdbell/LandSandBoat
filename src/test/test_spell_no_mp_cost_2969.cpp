#include "test_spell_no_mp_cost_2969.h"

#include "map/spell_cost_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spell ShouldReturnZeroNoMPCost 2969 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CalculateSpellCost hasMPCost gate for dual-wire cross-check
// (slice 2969):
//   !hasMPCost
auto inlineShouldReturnZeroNoMPCost(const bool hasMPCost) -> bool
{
    return !hasMPCost;
}

} // namespace

// Pure dual-wire expansion for spellcosthelpers::ShouldReturnZeroNoMPCost
// (!hasMPCost; slice 2969).
auto runSpellNoMPCost2969SelfTests() -> bool
{
    using spellcosthelpers::ShouldReturnZeroNoMPCost;
    using spellcosthelpers::ShouldReturnZeroNullSpell;

    bool ok = true;

    // Residual 2104 truth-table pins.
    ok = expect(ShouldReturnZeroNoMPCost(false), "residual: no MP cost returns zero") && ok;
    ok = expect(!ShouldReturnZeroNoMPCost(true), "residual: has MP cost proceeds") && ok;

    const struct
    {
        bool        hasMPCost;
        bool        want;
        const char* label;
    } cases[] = {
        { false, true, "no MP cost returns zero" },
        { true, false, "has MP cost proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldReturnZeroNoMPCost(c.hasMPCost);
        const bool inlineF = inlineShouldReturnZeroNoMPCost(c.hasMPCost);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReturnZeroNoMPCost dual-wire == inline LSB formula") && ok;
        ok = expect(got == !c.hasMPCost, "ShouldReturnZeroNoMPCost == pin formula") && ok;
    }

    // Pin composition: negation of hasMPCost only.
    ok = expect(ShouldReturnZeroNoMPCost(false), "!hasMPCost must return zero") && ok;
    ok = expect(!ShouldReturnZeroNoMPCost(true), "hasMPCost must not return zero") && ok;

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

        ok = expect(got == c.wantZero, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
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
    }

    // Residual independence (2104 / 2964): null zero and no-MP-cost zero both
    // block via different pure helpers.
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
