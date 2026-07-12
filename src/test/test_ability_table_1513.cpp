#include "test_ability_table_1513.h"

#include "map/ability_table_capacity.h"

#include <iostream>

namespace
{
using abilitytablehelpers::AbilityHealingRuby;
using abilitytablehelpers::AbilityPetCommands;
using abilitytablehelpers::AddTypeAvatar;
using abilitytablehelpers::AddTypeCharmPet;
using abilitytablehelpers::AddTypeJugPet;
using abilitytablehelpers::AddTypeMainOnly;
using abilitytablehelpers::AddTypeMerit;
using abilitytablehelpers::AvatarAddtypeOK;
using abilitytablehelpers::ChargeTimeAfterMerit;
using abilitytablehelpers::CheckAbilityAddtype;
using abilitytablehelpers::MeritAddtypeOK;
using abilitytablehelpers::ShouldAddMainAbility;
using abilitytablehelpers::ShouldAddSubAbility;
using abilitytablehelpers::ShouldInitAbilityRecast;
using abilitytablehelpers::ShouldRejectNullChar;
using abilitytablehelpers::ShouldSkipSubJobAbilities;
using abilitytablehelpers::ShouldStopMainAbilityLoop;
using abilitytablehelpers::ShouldStopSubAbilityLoop;

auto Check() -> bool
{
    if (!ShouldRejectNullChar(true) || ShouldRejectNullChar(false))
    {
        return false;
    }
    if (!ShouldStopMainAbilityLoop(10, 20) || ShouldStopMainAbilityLoop(20, 10))
    {
        return false;
    }
    if (!ShouldAddMainAbility(54, true) || ShouldAddMainAbility(AbilityPetCommands, true) || ShouldAddMainAbility(AbilityHealingRuby, true) ||
        ShouldAddMainAbility(10, false))
    {
        return false;
    }
    if (!ShouldSkipSubJobAbilities(0) || ShouldSkipSubJobAbilities(1))
    {
        return false;
    }
    if (!ShouldStopSubAbilityLoop(5, 10) || ShouldStopSubAbilityLoop(10, 5))
    {
        return false;
    }
    if (!ShouldAddSubAbility(1, 10, 0, true) || ShouldAddSubAbility(0, 10, 0, true) || ShouldAddSubAbility(1, 10, AddTypeMainOnly, true) ||
        ShouldAddSubAbility(1, AbilityPetCommands, 0, true))
    {
        return false;
    }
    if (!MeritAddtypeOK(0, false, false) || !MeritAddtypeOK(AddTypeMerit, true, true) || MeritAddtypeOK(AddTypeMerit, true, false) ||
        MeritAddtypeOK(AddTypeMerit, false, true))
    {
        return false;
    }
    if (!AvatarAddtypeOK(0, false, false, 0) || !AvatarAddtypeOK(AddTypeAvatar, true, true, 1) ||
        AvatarAddtypeOK(AddTypeAvatar, true, true, 17) || AvatarAddtypeOK(AddTypeAvatar, true, false, 1))
    {
        return false;
    }
    if (!CheckAbilityAddtype(0, false, false, false, false, false, false, false, false, false, false, false, 0, false))
    {
        return false;
    }
    if (CheckAbilityAddtype(AddTypeMerit, false, false, false, false, false, false, false, false, false, false, false, 0, false))
    {
        return false;
    }
    if (!CheckAbilityAddtype(AddTypeMerit, true, true, false, false, false, false, false, false, false, false, false, 0, false))
    {
        return false;
    }
    if (ChargeTimeAfterMerit(30, 5) != 25)
    {
        return false;
    }
    if (!ShouldInitAbilityRecast(false) || ShouldInitAbilityRecast(true))
    {
        return false;
    }
    // jug+charm both bits
    if (!CheckAbilityAddtype(AddTypeJugPet | AddTypeCharmPet, false, false, false, false, false, false, true, true, false, false, false, 0, false))
    {
        return false;
    }
    if (CheckAbilityAddtype(AddTypeJugPet | AddTypeCharmPet, false, false, false, false, false, false, false, false, false, false, false, 0, false))
    {
        return false;
    }
    return true;
}
} // namespace

auto runAbilityTable1513SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "ability table 1513 self-test failed\n";
    }
    return ok;
}
