#include "test_arrow_usage_for_scavenge_6852.h"

#include "map/entities/char_entity.h"
#include "map/items/item_weapon.h"
#include "map/utils/charutils.h"

#include <iostream>

namespace
{
auto Check() -> bool
{
    auto character = CCharEntity{};
    auto firstAmmo = CItemWeapon{ 599 };
    auto nextAmmo  = CItemWeapon{ 600 };

    charutils::TrackArrowUsageForScavenge(&character, &firstAmmo);
    if (character.GetLocalVar("ArrowsUsed") != 5'990'001)
    {
        return false;
    }

    charutils::TrackArrowUsageForScavenge(&character, &firstAmmo);
    if (character.GetLocalVar("ArrowsUsed") != 5'990'002)
    {
        return false;
    }

    character.SetLocalVar("ArrowsUsed", 5'991'980);
    charutils::TrackArrowUsageForScavenge(&character, &firstAmmo);
    if (character.GetLocalVar("ArrowsUsed") != 5'991'980)
    {
        return false;
    }

    charutils::TrackArrowUsageForScavenge(&character, &nextAmmo);
    return character.GetLocalVar("ArrowsUsed") == 6'000'001;
}
} // namespace

auto runArrowUsageForScavenge6852SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "arrow usage for scavenge 6852 self-test failed\\n";
    }
    return ok;
}
