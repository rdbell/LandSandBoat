#include "test_enmity_ws_use_1560.h"

#include "map/enmity_mod_capacity.h"
#include "map/weaponskill_use_capacity.h"

#include <iostream>

namespace
{
using enmitymodhelpers::GetEnmityModCure;
using enmitymodhelpers::GetEnmityModDamage;
using weaponskillusehelpers::CanUseWeaponskill;
using weaponskillusehelpers::CanUseWeaponskillInput;
using weaponskillusehelpers::IsValidSelfTargetWeaponskill;

auto Check() -> bool
{
    // EnmityModDamage: level*31/50+6
    if (GetEnmityModDamage(0) != 6 || GetEnmityModDamage(50) != 37 || GetEnmityModDamage(75) != 52 ||
        GetEnmityModDamage(99) != 67)
    {
        return false;
    }

    // EnmityModCure bands
    if (GetEnmityModCure(1) != 11 || GetEnmityModCure(10) != 20)
    {
        return false;
    }
    // level 20: 20 + (20-10)/2 = 25
    if (GetEnmityModCure(20) != 25 || GetEnmityModCure(50) != 40)
    {
        return false;
    }
    // level 75: int16(40 + 25*0.6) = int16(55) = 55
    if (GetEnmityModCure(75) != 55)
    {
        return false;
    }
    // level 99: int16(40 + 49*0.6) = int16(69.4) = 69
    if (GetEnmityModCure(99) != 69)
    {
        return false;
    }

    // Self-target WS allowlist
    if (!IsValidSelfTargetWeaponskill(163) || !IsValidSelfTargetWeaponskill(164) ||
        !IsValidSelfTargetWeaponskill(173) || !IsValidSelfTargetWeaponskill(190))
    {
        return false;
    }
    if (IsValidSelfTargetWeaponskill(-1) || IsValidSelfTargetWeaponskill(0) ||
        IsValidSelfTargetWeaponskill(1) || IsValidSelfTargetWeaponskill(162) || IsValidSelfTargetWeaponskill(256))
    {
        return false;
    }

    // CanUse: skill level path
    {
        CanUseWeaponskillInput in{};
        in.skillLevel = 100;
        in.charSkill  = 100;
        in.unlockID   = 0;
        in.mainJobReq = 1;
        if (!CanUseWeaponskill(in))
        {
            return false;
        }
        in.charSkill = 99;
        if (CanUseWeaponskill(in))
        {
            return false;
        }
    }
    // skill 0 + unlock + learned + mlevel 75
    {
        CanUseWeaponskillInput in{};
        in.skillLevel = 0;
        in.unlockID   = 1;
        in.hasLearned = true;
        in.mLevel     = 75;
        in.mainJobReq = 1;
        if (!CanUseWeaponskill(in))
        {
            return false;
        }
        in.mLevel = 74;
        if (CanUseWeaponskill(in))
        {
            return false;
        }
    }
    // job gate: sub only when !mainOnly
    {
        CanUseWeaponskillInput in{};
        in.skillLevel = 0;
        in.unlockID   = 0;
        in.mainJobReq = 0;
        in.subJobReq  = 1;
        in.mainOnly   = true;
        if (CanUseWeaponskill(in))
        {
            return false;
        }
        in.mainOnly = false;
        if (!CanUseWeaponskill(in))
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runEnmityWSUse1560SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "enmity_ws_use_1560 self-tests failed\n";
        return false;
    }
    return true;
}
