#include "test_mob_seal_pool_2698.h"

#include "map/entities/mob_seal_pool.h"

#include <iostream>
#include <vector>

auto runMobSealPool2698SelfTests() -> bool
{
    using mobsealpoolhelpers::EligibleSeals;
    const bool ok =
        EligibleSeals(0, false) == std::vector<uint16>{ BEASTMENS_SEAL } &&
        EligibleSeals(49, true) == std::vector<uint16>{ BEASTMENS_SEAL } &&
        EligibleSeals(50, false) == std::vector<uint16>{ BEASTMENS_SEAL, KINDREDS_SEAL } &&
        EligibleSeals(69, true) == std::vector<uint16>{ BEASTMENS_SEAL, KINDREDS_SEAL } &&
        EligibleSeals(70, false) == std::vector<uint16>{ BEASTMENS_SEAL, KINDREDS_SEAL } &&
        EligibleSeals(70, true) == std::vector<uint16>{ BEASTMENS_SEAL, KINDREDS_SEAL, KINDREDS_CREST } &&
        EligibleSeals(79, true) == std::vector<uint16>{ BEASTMENS_SEAL, KINDREDS_SEAL, KINDREDS_CREST } &&
        EligibleSeals(80, false) == std::vector<uint16>{ BEASTMENS_SEAL, KINDREDS_SEAL } &&
        EligibleSeals(80, true) == std::vector<uint16>{ BEASTMENS_SEAL, KINDREDS_SEAL, KINDREDS_CREST, HIGH_KINDREDS_CREST };
    if (!ok)
    {
        std::cerr << "mob seal pool 2698 failed\n";
    }
    return ok;
}
