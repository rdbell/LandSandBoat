#include "test_monstrosity_instinct_loadout_3247.h"

#include "map/monstrosity_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "monstrosity ShouldRejectInstinctLoadout 3247 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline HandleEquipChangePacket cost/duplicate reject formula for dual-wire
// cross-check (dedicated 3247 expand residual 2872 / prior dedicated 3168):
//   hasDuplicates || totalCost > maxPoints
auto inlineShouldRejectInstinctLoadout(const uint16 totalCost, const uint16 maxPoints, const bool hasDuplicates) -> bool
{
    return hasDuplicates || totalCost > maxPoints;
}

// Compact dual-wire pin matching Go pinShouldRejectInstinctLoadout3247 /
// C++ capacity direct-return form:
//   hasDuplicates || totalCost > maxPoints
auto pinShouldRejectInstinctLoadout(const uint16 totalCost, const uint16 maxPoints, const bool hasDuplicates) -> bool
{
    return hasDuplicates || totalCost > maxPoints;
}

} // namespace

// Pure dual-wire expansion for monstrosityhelpers::ShouldRejectInstinctLoadout
// (HandleEquipChangePacket InstinctFlag cost/duplicate gate;
// OmegaXI internal/monstrosity; dedicated slice 3247 expand residual 2872 /
// prior dedicated 3168 — formula unchanged).
//
// Coverage:
//   - free == inline == pin == (hasDuplicates || totalCost > maxPoints)
//     (pin direct return)
//   - residual 2766 / 2872 / prior 3168 pins still hold
//   - residual poles + dense: hasDuplicates × cost vs max poles
auto runMonstrosityInstinctLoadout3247SelfTests() -> bool
{
    using monstrosityhelpers::ShouldRejectInstinctLoadout;

    bool ok = true;

    // Residual 2766 / 2872 / prior 3168 pins still hold under dual-wire.
    ok = expect(!ShouldRejectInstinctLoadout(10, 11, false), "residual under budget no dups → accept") && ok;
    ok = expect(ShouldRejectInstinctLoadout(12, 11, false), "residual over budget → reject") && ok;
    ok = expect(ShouldRejectInstinctLoadout(0, 11, true), "residual dups under budget → reject") && ok;
    ok = expect(ShouldRejectInstinctLoadout(100, 10, true), "residual both over+dups → reject") && ok;
    ok = expect(!ShouldRejectInstinctLoadout(11, 11, false), "residual exact budget → accept") && ok;
    ok = expect(!ShouldRejectInstinctLoadout(0, 0, false), "residual zero cost / zero max → accept") && ok;
    ok = expect(ShouldRejectInstinctLoadout(1, 0, false), "residual any cost over zero max → reject") && ok;
    ok = expect(ShouldRejectInstinctLoadout(11, 11, true), "residual exact budget but dups → reject") && ok;

    // --- Composition table: free == inline == pin ---
    // Residual poles: cost==max, cost>max, duplicates, under, both.
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
        { 65535, 65535, false, false, "cost==max uint16 max" },
        // cost > max → reject
        { 12, 11, false, true, "cost>max over budget" },
        { 1, 0, false, true, "cost>max any over zero max" },
        { 65535, 65534, false, true, "cost>max uint16 max over" },
        // duplicates → reject (even under / exact budget)
        { 0, 11, true, true, "duplicates under budget" },
        { 11, 11, true, true, "duplicates at exact budget" },
        { 0, 0, true, true, "duplicates zero cost/max" },
        // Accept poles (no dups, under budget)
        { 10, 11, false, false, "under budget no dups" },
        { 0, 10, false, false, "empty under budget" },
        { 50, 60, false, false, "comfortable under" },
        // Both failure modes
        { 100, 10, true, true, "both over+dups" },
        { 12, 11, true, true, "both over+dups L1" },
        // Residual 2872 / prior 3168 large over
        { 100, 10, false, true, "large over budget no dups" },
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

    // Dense residual poles: hasDuplicates × cost-vs-max free == inline == pin.
    // Cost pairs cover under / exact / over / zero / uint16 edges around L1
    // budget (maxPoints = 11 = InstinctMaxPoints(1)).
    const struct
    {
        uint16 totalCost;
        uint16 maxPoints;
    } costPoles[] = {
        { 10, 11 },       // under L1
        { 11, 11 },       // exact L1
        { 12, 11 },       // over L1
        { 0, 11 },        // empty under L1
        { 0, 0 },         // zero exact
        { 1, 0 },         // over zero max
        { 100, 10 },      // large over
        { 50, 60 },       // comfortable under
        { 65535, 65534 }, // uint16 max over
        { 65535, 65535 }, // uint16 max exact
        { 0, 65535 },     // empty under max uint16
        { 65534, 65535 }, // under max-1
    };

    for (const bool hasDuplicates : { false, true })
    {
        for (const auto& cp : costPoles)
        {
            const bool got     = ShouldRejectInstinctLoadout(cp.totalCost, cp.maxPoints, hasDuplicates);
            const bool inlineF = inlineShouldRejectInstinctLoadout(cp.totalCost, cp.maxPoints, hasDuplicates);
            const bool pin     = pinShouldRejectInstinctLoadout(cp.totalCost, cp.maxPoints, hasDuplicates);
            const bool want    = hasDuplicates || cp.totalCost > cp.maxPoints;

            ok = expect(got == want, "dense free == pin formula") && ok;
            ok = expect(got == inlineF && got == pin, "dense free == inline == pin") && ok;
        }
    }

    // Dense cost sweep around L1 maxPoints=11 (under/exact/over neighbors)
    // for both hasDuplicates values: free == inline == pin.
    constexpr uint16 l1Max = 11;
    for (const bool hasDuplicates : { false, true })
    {
        for (uint16 cost = 0; cost <= l1Max + 5; ++cost)
        {
            const bool got     = ShouldRejectInstinctLoadout(cost, l1Max, hasDuplicates);
            const bool inlineF = inlineShouldRejectInstinctLoadout(cost, l1Max, hasDuplicates);
            const bool pin     = pinShouldRejectInstinctLoadout(cost, l1Max, hasDuplicates);
            const bool want    = hasDuplicates || cost > l1Max;

            ok = expect(got == want, "dense L1 sweep free == formula") && ok;
            ok = expect(got == inlineF && got == pin, "dense L1 sweep free == inline == pin") && ok;
        }
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
        { 11, 11, true },  // exact + dups
        { 0, 0, false },   // zero exact
        { 1, 0, false },   // over zero
        { 100, 10, true }, // both
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

    // Production HandleEquipChangePacket path semantics (host inject model).
    // Level-1 family → maxPoints = level + 10 = 11.
    constexpr uint16 maxPoints = 11;
    ok = expect(!ShouldRejectInstinctLoadout(10, maxPoints, false), "host inject under unique accept") && ok;
    ok = expect(ShouldRejectInstinctLoadout(12, maxPoints, false), "host inject over unique reject") && ok;
    ok = expect(ShouldRejectInstinctLoadout(0, maxPoints, true), "host inject dups reject") && ok;
    ok = expect(!ShouldRejectInstinctLoadout(maxPoints, maxPoints, false), "host inject exact budget accept") && ok;

    // Explicit dual-wire: free == inline == pin for host poles.
    for (const auto& p : poles)
    {
        const bool got     = ShouldRejectInstinctLoadout(p.totalCost, p.maxPoints, p.hasDuplicates);
        const bool inlineF = inlineShouldRejectInstinctLoadout(p.totalCost, p.maxPoints, p.hasDuplicates);
        const bool pin     = pinShouldRejectInstinctLoadout(p.totalCost, p.maxPoints, p.hasDuplicates);
        ok                 = expect(got == pin, "host inject free == pin") && ok;
        ok                 = expect(got == inlineF, "host inject free == inline") && ok;
    }

    return ok;
}
