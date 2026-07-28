#include "test_monstrosity_instinct_costs_7497.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityInstinctCosts7497SelfTests() -> bool
{
    using monstrosity::InstinctCatalog;
    using monstrosity::InstinctCatalogRow;
    using monstrosity::ResolveEquippedInstinctCosts;

    auto catalog = InstinctCatalog{
        { 10, { .monstrosityInstinctId = 10, .cost = 3 } },
        { 11, { .monstrosityInstinctId = 11, .cost = 7 } },
    };
    std::array<uint16, 12> equipped{};
    equipped[0] = 10;
    equipped[1] = 99;
    equipped[3] = 11;

    const auto costs = ResolveEquippedInstinctCosts(catalog, equipped);
    if (costs[0] != 3 || costs[1] != 0 || costs[2] != 0 || costs[3] != 7)
    {
        std::cerr << "monstrosity instinct costs: catalog costs and empty slots failed\n";
        return false;
    }
    if (!catalog.contains(99) || catalog.at(99).cost != 0 || !catalog.contains(0) || catalog.at(0).cost != 0)
    {
        std::cerr << "monstrosity instinct costs: missing IDs should default-insert at zero cost\n";
        return false;
    }
    return true;
}
