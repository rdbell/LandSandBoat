#include "test_monstrosity_species_catalog_7492.h"

#include "map/monstrosity.h"

#include <iostream>
#include <vector>

auto runMonstrositySpeciesCatalog7492SelfTests() -> bool
{
    using monstrosity::ApplySpeciesCatalogRows;
    using monstrosity::SpeciesCatalog;
    using monstrosity::SpeciesCatalogRow;

    auto catalog = SpeciesCatalog{
        { 40, { .monstrosityId = 4, .monstrositySpeciesCode = 40, .name = "Existing", .mjob = JOB_WAR, .sjob = JOB_WAR, .size = 1, .look = 0x0100 } },
    };
    ApplySpeciesCatalogRows(catalog, std::vector<SpeciesCatalogRow>{
                                         { .monstrosityId = 7, .monstrositySpeciesCode = 3, .name = "Old", .mjob = JOB_MNK, .sjob = JOB_WAR, .size = 0, .look = 0x0200 },
                                         { .monstrosityId = 8, .monstrositySpeciesCode = 3, .name = "Replacement", .mjob = JOB_WAR, .sjob = JOB_MNK, .size = 2, .look = 0x0300 },
                                     });

    if (catalog.size() != 2 || catalog[40].name != "Existing")
    {
        std::cerr << "monstrosity species catalog: existing entries should be retained\n";
        return false;
    }

    const auto& species = catalog[3];
    if (species.monstrosityId != 8 || species.monstrositySpeciesCode != 3 || species.name != "Replacement" || species.mjob != JOB_WAR || species.sjob != JOB_MNK || species.size != 2 || species.look != 0x0300)
    {
        std::cerr << "monstrosity species catalog: rows should replace by species code\n";
        return false;
    }

    return true;
}
