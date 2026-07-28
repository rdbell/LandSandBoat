#include "test_monstrosity_instinct_loadout_plan_7498.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityInstinctLoadoutPlan7498SelfTests() -> bool
{
    using monstrosity::InstinctCatalog;
    using monstrosity::InstinctCatalogRow;
    using monstrosity::PlanInstinctLoadout;

    auto catalog = InstinctCatalog{
        { 10, { .monstrosityInstinctId = 10, .cost = 3 } },
        { 11, { .monstrosityInstinctId = 11, .cost = 7 } },
        { 12, { .monstrosityInstinctId = 12, .cost = 1 } },
    };

    std::array<uint16, 12> exactSlots{};
    exactSlots[0] = 10;
    exactSlots[1] = 11;
    const auto exact = PlanInstinctLoadout(catalog, exactSlots, 0);
    if (exact.costs[0] != 3 || exact.costs[1] != 7 || exact.totalCost != 10 || exact.maxPoints != 10 || exact.hasDuplicates || exact.rejectLoadout)
    {
        std::cerr << "monstrosity instinct loadout plan: exact budget should be accepted\n";
        return false;
    }

    auto overSlots = exactSlots;
    overSlots[2] = 12;
    const auto over = PlanInstinctLoadout(catalog, overSlots, 0);
    if (over.totalCost != 11 || over.maxPoints != 10 || over.hasDuplicates || !over.rejectLoadout)
    {
        std::cerr << "monstrosity instinct loadout plan: over-budget loadout should reject\n";
        return false;
    }

    std::array<uint16, 12> duplicateSlots{};
    duplicateSlots[0] = 10;
    duplicateSlots[1] = 10;
    const auto duplicate = PlanInstinctLoadout(catalog, duplicateSlots, 99);
    if (duplicate.totalCost != 6 || duplicate.maxPoints != 109 || !duplicate.hasDuplicates || !duplicate.rejectLoadout)
    {
        std::cerr << "monstrosity instinct loadout plan: duplicate loadout should reject\n";
        return false;
    }

    return true;
}
