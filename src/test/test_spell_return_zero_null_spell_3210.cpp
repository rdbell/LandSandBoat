#include "test_spell_return_zero_null_spell_3210.h"

#include "map/spell_cost_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spell ShouldReturnZeroNullSpell 3210 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CalculateSpellCost null-PSpell gate for dual-wire cross-check
// (dedicated slice 3210):
//   spellNull
auto inlineShouldReturnZeroNullSpell(const bool spellNull) -> bool
{
    return spellNull;
}

// Compact dual-wire pin matching Go pinShouldReturnZeroNullSpell3210 /
// C++ capacity:
//   spellNull
auto pinShouldReturnZeroNullSpell(const bool spellNull) -> bool
{
    return spellNull;
}

} // namespace

// Pure dual-wire expansion for spellcosthelpers::ShouldReturnZeroNullSpell
// (spellNull identity; dedicated slice 3210 expand residual 2964 / pure 2104).
//
// Coverage:
//   - free == inline == pin == spellNull
//   - residual 2104 / 2964 pins still hold
//   - both bool poles (spellNull false/true)
//   - sibling residual independence (no-MP-cost 3183 not re-expanded)
auto runSpellReturnZeroNullSpell3210SelfTests() -> bool
{
    using spellcosthelpers::ShouldReturnZeroNoMPCost;
    using spellcosthelpers::ShouldReturnZeroNullSpell;

    bool ok = true;

    // Residual 2104 / 2964 pins still hold under dual-wire.
    ok = expect(ShouldReturnZeroNullSpell(true), "residual 2964/2104: null spell returns zero") && ok;
    ok = expect(!ShouldReturnZeroNullSpell(false), "residual 2964/2104: non-null spell proceeds") && ok;

    const struct
    {
        bool        spellNull;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "pole null spell → return zero" },
        { false, false, "pole non-null spell → proceed" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldReturnZeroNullSpell(c.spellNull);
        const bool inlineF = inlineShouldReturnZeroNullSpell(c.spellNull);
        const bool pinF    = pinShouldReturnZeroNullSpell(c.spellNull);
        const bool wantPin = c.spellNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReturnZeroNullSpell dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldReturnZeroNullSpell dual-wire == pin formula") && ok;
        ok = expect(got == wantPin, "ShouldReturnZeroNullSpell == pin formula spellNull") && ok;
    }

    // Pin composition: identity of spellNull only.
    ok = expect(ShouldReturnZeroNullSpell(true), "spellNull must return zero") && ok;
    ok = expect(!ShouldReturnZeroNullSpell(false), "!spellNull must not return zero") && ok;

    // Explicit polarity: zero is exact identity of inject value.
    for (const bool spellNull : { false, true })
    {
        const bool got  = ShouldReturnZeroNullSpell(spellNull);
        const bool want = spellNull;
        ok              = expect(got == want, "polarity: free == spellNull") && ok;
        ok              = expect(!(got && !spellNull), "polarity: never zero when !spellNull") && ok;
        ok              = expect(!(!got && spellNull), "polarity: always zero when spellNull") && ok;
    }

    // Host path: before hasMPCost / pure cost body.
    const struct
    {
        bool        spellNull;
        bool        wantZero;
        const char* label;
    } hostCases[] = {
        { true, true, "PSpell == nullptr → return zero before cost" },
        { false, false, "PSpell non-null → proceed to hasMPCost / cost" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldReturnZeroNullSpell(c.spellNull);
        const bool inlineF = inlineShouldReturnZeroNullSpell(c.spellNull);
        const bool pinF    = pinShouldReturnZeroNullSpell(c.spellNull);

        ok = expect(got == c.wantZero, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinF, "host compose dual-wire free == pin") && ok;
        ok = expect(got == c.spellNull, "host compose free == spellNull (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool spellNull : { false, true })
    {
        const bool got  = ShouldReturnZeroNullSpell(spellNull);
        const bool want = spellNull;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldReturnZeroNullSpell(spellNull),
                    "compose free == inline") &&
             ok;
        ok = expect(got == pinShouldReturnZeroNullSpell(spellNull), "compose free == pin") && ok;
    }

    // Residual independence (2104 / 2964 / 3183): null zero and no-MP-cost
    // zero both block via different pure helpers. Sibling 3183 left alone.
    ok = expect(ShouldReturnZeroNullSpell(true) && ShouldReturnZeroNoMPCost(false),
                "null zero and no-MP-cost zero both block") &&
         ok;
    ok = expect(!ShouldReturnZeroNullSpell(false),
                "non-null must proceed even if later hasMPCost zeros") &&
         ok;
    ok = expect(!ShouldReturnZeroNoMPCost(true),
                "hasMPCost residual still holds under dual-wire") &&
         ok;

    return ok;
}
