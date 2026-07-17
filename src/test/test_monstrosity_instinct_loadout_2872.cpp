#include "test_monstrosity_instinct_loadout_2872.h"

#include "map/monstrosity_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "monstrosity ShouldRejectInstinctLoadout 2872 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline HandleEquipChangePacket cost/duplicate reject formula for dual-wire
// cross-check (slice 2872 / 2766):
//   hasDuplicates || totalCost > maxPoints
auto inlineShouldRejectInstinctLoadout(const uint16 totalCost, const uint16 maxPoints, const bool hasDuplicates) -> bool
{
    return hasDuplicates || totalCost > maxPoints;
}

} // namespace

// Pure dual-wire expansion for monstrosityhelpers::ShouldRejectInstinctLoadout
// (HandleEquipChangePacket InstinctFlag cost/duplicate gate).
auto runMonstrosityInstinctLoadout2872SelfTests() -> bool
{
    using monstrosityhelpers::ShouldRejectInstinctLoadout;

    bool ok = true;

    const struct
    {
        uint16      totalCost;
        uint16      maxPoints;
        bool        hasDuplicates;
        bool        want;
        const char* label;
    } cases[] = {
        // Accept: under budget, no dups
        { 0, 10, false, false, "empty under budget" },
        { 10, 11, false, false, "under budget residual 2766" },
        { 11, 11, false, false, "exact budget accepted" },
        { 0, 0, false, false, "zero cost / zero max" },
        { 50, 60, false, false, "comfortable under" },
        // Reject: over budget only
        { 12, 11, false, true, "over budget residual 2766" },
        { 1, 0, false, true, "any cost over zero max" },
        { 100, 10, false, true, "large over budget" },
        { 65535, 65534, false, true, "uint16 max over" },
        // Reject: duplicates only
        { 0, 11, true, true, "dups under budget residual 2766" },
        { 0, 0, true, true, "dups zero cost/max" },
        { 5, 10, true, true, "dups under budget" },
        // Reject: both failure modes
        { 100, 10, true, true, "both over+dups residual 2766" },
        { 12, 11, true, true, "both over+dups" },
        // Boundary: exact budget with dups still rejects
        { 11, 11, true, true, "exact budget but dups" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectInstinctLoadout(c.totalCost, c.maxPoints, c.hasDuplicates);
        const bool inlineF = inlineShouldRejectInstinctLoadout(c.totalCost, c.maxPoints, c.hasDuplicates);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline formula") && ok;
    }

    // Compose dual-wire pins: level-1 budget (maxPoints = 1+10 = 11) as in
    // HandleEquipChangePacket InstinctMaxPoints(level).
    // under / exact / over without dups.
    ok = expect(!ShouldRejectInstinctLoadout(10, 11, false), "compose under L1 budget") && ok;
    ok = expect(!ShouldRejectInstinctLoadout(11, 11, false), "compose exact L1 budget") && ok;
    ok = expect(ShouldRejectInstinctLoadout(12, 11, false), "compose over L1 budget") && ok;
    ok = expect(ShouldRejectInstinctLoadout(0, 11, true), "compose dups L1 budget") && ok;
    ok = expect(ShouldRejectInstinctLoadout(10, 11, false) ==
                    inlineShouldRejectInstinctLoadout(10, 11, false),
                "compose under dual-wire") &&
         ok;

    return ok;
}
