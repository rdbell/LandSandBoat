#include "test_monstrosity_instinct_catalog_7493.h"

#include "map/monstrosity.h"

#include <iostream>
#include <vector>

auto runMonstrosityInstinctCatalog7493SelfTests() -> bool
{
    using monstrosity::ApplyInstinctCatalogRows;
    using monstrosity::InstinctCatalog;
    using monstrosity::InstinctCatalogRow;

    auto catalog = InstinctCatalog{
        { 40, { .monstrosityInstinctId = 40, .cost = 1, .name = "Existing" } },
    };
    ApplyInstinctCatalogRows(catalog, std::vector<InstinctCatalogRow>{
                                          { .monstrosityInstinctId = 3, .cost = 2, .name = "Old" },
                                          { .monstrosityInstinctId = 3, .cost = 5, .name = "Replacement" },
                                      });

    if (catalog.size() != 2 || catalog[40].cost != 1 || catalog[40].name != "Existing")
    {
        std::cerr << "monstrosity instinct catalog: existing entries should be retained\n";
        return false;
    }

    const auto& instinct = catalog[3];
    if (instinct.monstrosityInstinctId != 3 || instinct.cost != 5 || instinct.name != "Replacement")
    {
        std::cerr << "monstrosity instinct catalog: rows should replace by instinct ID\n";
        return false;
    }

    return true;
}
