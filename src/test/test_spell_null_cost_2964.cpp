#include "test_spell_null_cost_2964.h"

#include "map/spell_cost_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spell ShouldReturnZeroNullSpell 2964 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CalculateSpellCost null-PSpell gate for dual-wire cross-check
// (slice 2964):
//   spellNull
auto inlineShouldReturnZeroNullSpell(const bool spellNull) -> bool
{
    return spellNull;
}

} // namespace

// Pure dual-wire expansion for spellcosthelpers::ShouldReturnZeroNullSpell
// (spellNull identity; slice 2964).
auto runSpellNullCost2964SelfTests() -> bool
{
    using spellcosthelpers::ShouldReturnZeroNoMPCost;
    using spellcosthelpers::ShouldReturnZeroNullSpell;

    bool ok = true;

    // Residual 2104 truth-table pins.
    ok = expect(ShouldReturnZeroNullSpell(true), "residual: null spell returns zero") && ok;
    ok = expect(!ShouldReturnZeroNullSpell(false), "residual: non-null spell proceeds") && ok;

    const struct
    {
        bool        spellNull;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "null spell returns zero" },
        { false, false, "non-null spell proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldReturnZeroNullSpell(c.spellNull);
        const bool inlineF = inlineShouldReturnZeroNullSpell(c.spellNull);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReturnZeroNullSpell dual-wire == inline LSB formula") && ok;
        ok = expect(got == c.spellNull, "ShouldReturnZeroNullSpell == pin formula") && ok;
    }

    // Pin composition: identity of spellNull only.
    ok = expect(ShouldReturnZeroNullSpell(true), "spellNull must return zero") && ok;
    ok = expect(!ShouldReturnZeroNullSpell(false), "!spellNull must not return zero") && ok;

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

        ok = expect(got == c.wantZero, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
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
    }

    // Residual independence (2104): null zero and no-MP-cost zero both block
    // via different pure helpers.
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
