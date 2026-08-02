#include "test_ability_table_1513.h"

#include "map/ability_table_capacity.h"

#include <iostream>

namespace
{

using abilitytablehelpers::AbilityHealingRuby;
using abilitytablehelpers::AbilityPetCommands;
using abilitytablehelpers::AddTypeAstralFlow;
using abilitytablehelpers::AddTypeAutomaton;
using abilitytablehelpers::AddTypeAvatar;
using abilitytablehelpers::AddTypeCharmPet;
using abilitytablehelpers::AddTypeDarkArts;
using abilitytablehelpers::AddTypeJugPet;
using abilitytablehelpers::AddTypeLearned;
using abilitytablehelpers::AddTypeLightArts;
using abilitytablehelpers::AddTypeMainOnly;
using abilitytablehelpers::AddTypeMerit;
using abilitytablehelpers::AstralFlowAddtypeOK;
using abilitytablehelpers::AutomatonAddtypeOK;
using abilitytablehelpers::AvatarAddtypeOK;
using abilitytablehelpers::ChargeTimeAfterMerit;
using abilitytablehelpers::CharmOnlyAddtypeOK;
using abilitytablehelpers::CheckAbilityAddtype;
using abilitytablehelpers::DarkArtsAddtypeOK;
using abilitytablehelpers::JugCharmBothAddtypeOK;
using abilitytablehelpers::JugOnlyAddtypeOK;
using abilitytablehelpers::LearnedAddtypeOK;
using abilitytablehelpers::LightArtsAddtypeOK;
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
    if (!ShouldStopSubAbilityLoop(5, 10) || ShouldStopSubAbilityLoop(10, 5))
    {
        return false;
    }
    if (!ShouldAddSubAbility(1, 10, 0, true) || ShouldAddSubAbility(0, 10, 0, true) || ShouldAddSubAbility(1, 10, AddTypeMainOnly, true) ||
        ShouldAddSubAbility(1, AbilityPetCommands, 0, true) || ShouldAddSubAbility(1, 10, 0, false))
    {
        return false;
    }
    if (!MeritAddtypeOK(0, false, false) || !MeritAddtypeOK(AddTypeMerit, true, true) || MeritAddtypeOK(AddTypeMerit, true, false) ||
        MeritAddtypeOK(AddTypeMerit, false, true))
    {
        return false;
    }
    if (!AstralFlowAddtypeOK(0, false) || !AstralFlowAddtypeOK(AddTypeAstralFlow, true) || AstralFlowAddtypeOK(AddTypeAstralFlow, false))
    {
        return false;
    }
    if (!LearnedAddtypeOK(0, false) || !LearnedAddtypeOK(AddTypeLearned, true) || LearnedAddtypeOK(AddTypeLearned, false))
    {
        return false;
    }
    if (!LightArtsAddtypeOK(0, false) || !LightArtsAddtypeOK(AddTypeLightArts, true) || LightArtsAddtypeOK(AddTypeLightArts, false))
    {
        return false;
    }
    if (!DarkArtsAddtypeOK(0, false) || !DarkArtsAddtypeOK(AddTypeDarkArts, true) || DarkArtsAddtypeOK(AddTypeDarkArts, false))
    {
        return false;
    }
    if (!JugCharmBothAddtypeOK(0, false, false) || !JugCharmBothAddtypeOK(AddTypeJugPet | AddTypeCharmPet, true, true) ||
        JugCharmBothAddtypeOK(AddTypeJugPet | AddTypeCharmPet, false, true) || JugCharmBothAddtypeOK(AddTypeJugPet | AddTypeCharmPet, true, false))
    {
        return false;
    }
    if (!JugOnlyAddtypeOK(0, false, false) || !JugOnlyAddtypeOK(AddTypeJugPet, true, true) || JugOnlyAddtypeOK(AddTypeJugPet, false, true) ||
        JugOnlyAddtypeOK(AddTypeJugPet, true, false))
    {
        return false;
    }
    if (!CharmOnlyAddtypeOK(0, false, false) || !CharmOnlyAddtypeOK(AddTypeCharmPet, true, true) ||
        CharmOnlyAddtypeOK(AddTypeCharmPet, false, true) || CharmOnlyAddtypeOK(AddTypeCharmPet, true, false))
    {
        return false;
    }
    if (!AvatarAddtypeOK(0, false, false, 0) || !AvatarAddtypeOK(AddTypeAvatar, true, true, 1) ||
        AvatarAddtypeOK(AddTypeAvatar, true, true, 17) || AvatarAddtypeOK(AddTypeAvatar, true, true, 18) ||
        AvatarAddtypeOK(AddTypeAvatar, true, true, 19) || AvatarAddtypeOK(AddTypeAvatar, true, false, 1) ||
        AvatarAddtypeOK(AddTypeAvatar, false, true, 1))
    {
        return false;
    }
    if (!AutomatonAddtypeOK(0, false, false) || !AutomatonAddtypeOK(AddTypeAutomaton, true, true) ||
        AutomatonAddtypeOK(AddTypeAutomaton, false, true) || AutomatonAddtypeOK(AddTypeAutomaton, true, false))
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
    const auto allAddtypes = AddTypeMerit | AddTypeAstralFlow | AddTypeLearned | AddTypeLightArts | AddTypeDarkArts |
                             AddTypeJugPet | AddTypeCharmPet | AddTypeAvatar | AddTypeAutomaton;
    if (!CheckAbilityAddtype(allAddtypes, true, true, true, true, true, true, true, true, true, true, true, 1, true))
    {
        return false;
    }
    if (CheckAbilityAddtype(allAddtypes, true, true, false, true, true, true, true, true, true, true, true, 1, true) ||
        CheckAbilityAddtype(allAddtypes, true, true, true, false, true, true, true, true, true, true, true, 1, true) ||
        CheckAbilityAddtype(allAddtypes, true, true, true, true, false, true, true, true, true, true, true, 1, true) ||
        CheckAbilityAddtype(allAddtypes, true, true, true, true, true, false, true, true, true, true, true, 1, true) ||
        CheckAbilityAddtype(allAddtypes, true, true, true, true, true, true, false, true, true, true, true, 1, true) ||
        CheckAbilityAddtype(allAddtypes, true, true, true, true, true, true, true, false, true, true, true, 1, true) ||
        CheckAbilityAddtype(allAddtypes ^ AddTypeCharmPet, true, true, true, true, true, true, true, true, false, true, true, 1, true) ||
        CheckAbilityAddtype(allAddtypes ^ AddTypeJugPet, true, true, true, true, true, true, true, true, true, false, true, 1, true) ||
        CheckAbilityAddtype(allAddtypes, true, true, true, true, true, true, true, true, true, true, false, 1, true) ||
        CheckAbilityAddtype(allAddtypes, true, true, true, true, true, true, true, true, true, true, true, 17, true) ||
        CheckAbilityAddtype(allAddtypes, true, true, true, true, true, true, true, true, true, true, true, 1, false))
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
