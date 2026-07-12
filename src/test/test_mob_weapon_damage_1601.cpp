#include "test_mob_weapon_damage_1601.h"

#include "map/mob_weapon_damage_capacity.h"

#include <iostream>
#include <tuple>

namespace
{
using namespace mobweapondamagehelpers;

auto Check() -> bool
{
    // Vanilla region offsets
    const auto vanilla = RegionOffsets(RegionRonfaure);
    if (vanilla.first != 2 || vanilla.second != 5)
    {
        return false;
    }
    const auto toau = RegionOffsets(RegionWestAhtUrhgan);
    if (toau.first != 10 || toau.second != 12)
    {
        return false;
    }
    const auto wotg = RegionOffsets(RegionRonfaureFront);
    if (wotg.first != 11 || wotg.second != 13)
    {
        return false;
    }
    // default
    if (RegionOffsets(255).first != 2 || RegionOffsets(255).second != 5)
    {
        return false;
    }

    // Beginner non-NM: 2,5 → 1,4
    const auto adj = ApplyBeginnerZoneOffsetAdjust(2, 5, ZoneWestRonfaure, false);
    if (adj.first != 1 || adj.second != 4)
    {
        return false;
    }
    // NM excluded
    const auto nm = ApplyBeginnerZoneOffsetAdjust(2, 5, ZoneWestRonfaure, true);
    if (nm.first != 2 || nm.second != 5)
    {
        return false;
    }
    // Full plan
    const auto plan = PlanBaseWeaponDamage(RegionRonfaure, 50, ZoneWestRonfaure, false);
    if (std::get<0>(plan) != 50 || std::get<1>(plan) != 1 || std::get<2>(plan) != 4)
    {
        return false;
    }
    if (!IsBeginnerDamageZone(100) || IsBeginnerDamageZone(104))
    {
        return false;
    }
    return true;
}
} // namespace

auto runMobWeaponDamage1601SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "mob_weapon_damage_1601 self-tests failed\n";
        return false;
    }
    return true;
}
