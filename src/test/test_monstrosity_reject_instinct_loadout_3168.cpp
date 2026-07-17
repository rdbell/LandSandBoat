#include "test_monstrosity_reject_instinct_loadout_3168.h"

#include "map/monstrosity_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "monstrosity ShouldRejectInstinctLoadout 3168 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline HandleEquipChangePacket cost/duplicate reject formula for dual-wire
// cross-check (dedicated 3168):
//   hasDuplicates || totalCost > maxPoints
auto inlineShouldRejectInstinctLoadout(const uint16 totalCost, const uint16 maxPoints, const bool hasDuplicates) -> bool
{
    return hasDuplicates || totalCost > maxPoints;
}

// Compact dual-wire pin matching Go pinShouldRejectInstinctLoadout3168 / C++ capacity:
//   hasDuplicates || totalCost > maxPoints
auto pinShouldRejectInstinctLoadout(const uint16 totalCost, const uint16 maxPoints, const bool hasDuplicates) -> bool
{
    return hasDuplicates || totalCost > maxPoints;
}

} // namespace

// Pure dual-wire expansion for monstrosityhelpers::ShouldRejectInstinctLoadout
// (HandleEquipChangePacket InstinctFlag cost/duplicate gate;
// OmegaXI internal/monstrosity; dedicated slice 3168).
//
// Coverage:
//   - free == inline == pin == (hasDuplicates || totalCost > maxPoints)
//   - residual 2766 / 2872 pins still hold
//   - poles: cost==max (accept), cost>max (reject), duplicates (reject)
auto runMonstrosityRejectInstinctLoadout3168SelfTests() -> bool
{
    using monstrosityhelpers::ShouldRejectInstinctLoadout;

    bool ok = true;

    // Residual 2766 / 2872 pins still hold under dual-wire.
    ok = expect(!ShouldRejectInstinctLoadout(10, 11, false), "residual under budget no dups → accept") && ok;
    ok = expect(ShouldRejectInstinctLoadout(12, 11, false), "residual over budget → reject") && ok;
    ok = expect(ShouldRejectInstinctLoadout(0, 11, true), "residual dups under budget → reject") && ok;
    ok = expect(ShouldRejectInstinctLoadout(100, 10, true), "residual both over+dups → reject") && ok;
    ok = expect(!ShouldRejectInstinctLoadout(11, 11, false), "residual exact budget → accept") && ok;

    // --- Composition table: free == inline == pin ---
    // Required poles: cost==max, cost>max, duplicates.
    const struct
    {
        uint16      totalCost;
        uint16      maxPoints;
        bool        hasDuplicates;
        bool        want;
        const char* label;
    } cases[] = {
        // cost == max → accept (strict >)
        { 11, 11, false, false, "cost==max exact budget accepted" },
        { 0, 0, false, false, "cost==max zero" },
        // cost > max → reject
        { 12, 11, false, true, "cost>max over budget" },
        { 1, 0, false, true, "cost>max any over zero max" },
        // duplicates → reject (even under / exact budget)
        { 0, 11, true, true, "duplicates under budget" },
        { 11, 11, true, true, "duplicates at exact budget" },
        // Accept poles (no dups, under budget)
        { 10, 11, false, false, "under budget no dups" },
        // Both failure modes
        { 100, 10, true, true, "both over+dups" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectInstinctLoadout(c.totalCost, c.maxPoints, c.hasDuplicates);
        const bool inlineF = inlineShouldRejectInstinctLoadout(c.totalCost, c.maxPoints, c.hasDuplicates);
        const bool pin     = pinShouldRejectInstinctLoadout(c.totalCost, c.maxPoints, c.hasDuplicates);
        const bool formula = c.hasDuplicates || c.totalCost > c.maxPoints;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == formula, "free == formula") && ok;
    }

    // Compose identity: free == inline == pin for key budget poles.
    const struct
    {
        uint16 totalCost;
        uint16 maxPoints;
        bool   hasDuplicates;
    } poles[] = {
        { 11, 11, false }, // cost==max
        { 12, 11, false }, // cost>max
        { 0, 11, true },   // duplicates
        { 10, 11, false }, // under
    };

    for (const auto& p : poles)
    {
        const bool got     = ShouldRejectInstinctLoadout(p.totalCost, p.maxPoints, p.hasDuplicates);
        const bool inlineF = inlineShouldRejectInstinctLoadout(p.totalCost, p.maxPoints, p.hasDuplicates);
        const bool pin     = pinShouldRejectInstinctLoadout(p.totalCost, p.maxPoints, p.hasDuplicates);
        const bool want    = p.hasDuplicates || p.totalCost > p.maxPoints;

        ok = expect(got == want, "compose free == formula") && ok;
        ok = expect(got == inlineF, "compose free == inline") && ok;
        ok = expect(got == pin, "compose free == pin") && ok;
    }

    return ok;
}
