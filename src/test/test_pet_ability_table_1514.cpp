#include "test_pet_ability_table_1514.h"

#include "map/pet_ability_table_capacity.h"
#include "map/pet_ability_table_lifecycle_capacity.h"
#include "map/trait_roster_capacity.h"
#include "map/traits_source_capacity.h"

#include <iostream>

namespace
{
using petabilitytablehelpers::AvatarPetAbilityBit;
using petabilitytablehelpers::CarbunclePetAbilityBit;
using petabilitytablehelpers::ElementalAvatarAbilityInBand;
using petabilitytablehelpers::IsCaitSithAbility;
using petabilitytablehelpers::IsCarbuncleAbility;
using petabilitytablehelpers::IsDiabolosAbility;
using petabilitytablehelpers::IsElementalAvatarPet;
using petabilitytablehelpers::IsSirenAbility;
using petabilitytablehelpers::IsSpiritOrSirenPet;
using petabilitytablehelpers::IsSummonerJob;
using petabilitytablehelpers::JugPetAbilityBit;
using petabilitytablehelpers::ShouldCalculateBlueTraits;
using petabilitytablehelpers::ShouldClearPetCommandsOnly;
using petabilitytablehelpers::ShouldConsiderSMNPetAbility;
using petabilitytablehelpers::ShouldRejectNullPetOrChar;
using petabilitytablehelpers::SirenPetAbilityBit;

auto Check() -> bool
{
    if (!ShouldRejectNullPetOrChar(true, false) || !ShouldRejectNullPetOrChar(false, true) || ShouldRejectNullPetOrChar(false, false))
    {
        return false;
    }
    if (!ShouldClearPetCommandsOnly(0) || ShouldClearPetCommandsOnly(8))
    {
        return false;
    }
    const auto rejectedLifecycle = petabilitytablelifecyclehelpers::PlanFor({
        .petNull = true,
        .petID   = petabilitytablehelpers::PetIDCarbuncle,
    });
    const auto clearOnlyLifecycle = petabilitytablelifecyclehelpers::PlanFor({});
    const auto buildLifecycle = petabilitytablelifecyclehelpers::PlanFor({
        .petID = petabilitytablehelpers::PetIDCarbuncle,
    });
    if (!rejectedLifecycle.reject || rejectedLifecycle.clearPetCommands || rejectedLifecycle.buildAbilityRosters ||
        rejectedLifecycle.pushCommandPacket || clearOnlyLifecycle.reject || !clearOnlyLifecycle.clearPetCommands ||
        clearOnlyLifecycle.buildAbilityRosters || !clearOnlyLifecycle.pushCommandPacket || buildLifecycle.reject ||
        !buildLifecycle.clearPetCommands || !buildLifecycle.buildAbilityRosters || !buildLifecycle.pushCommandPacket)
    {
        return false;
    }
    if (!IsSummonerJob(15, 1) || !IsSummonerJob(1, 15) || IsSummonerJob(1, 2))
    {
        return false;
    }
    if (!IsSpiritOrSirenPet(8) || !IsSpiritOrSirenPet(20) || !IsSpiritOrSirenPet(76) || IsSpiritOrSirenPet(7))
    {
        return false;
    }
    if (!ShouldConsiderSMNPetAbility(50, 10, 8, true) || ShouldConsiderSMNPetAbility(5, 10, 8, true) || ShouldConsiderSMNPetAbility(50, 10, 8, false))
    {
        return false;
    }
    if (!IsCarbuncleAbility(512) || !IsCarbuncleAbility(773) || IsCarbuncleAbility(521))
    {
        return false;
    }
    if (CarbunclePetAbilityBit(512) != 0 || CarbunclePetAbilityBit(773) != 261)
    {
        return false;
    }
    if (!IsElementalAvatarPet(9) || !IsElementalAvatarPet(15) || IsElementalAvatarPet(8))
    {
        return false;
    }
    // Fenrir petID=9 → lo = 512+(1)*16=528, hi=512+2*16=544
    if (!ElementalAvatarAbilityInBand(528, 9) || ElementalAvatarAbilityInBand(544, 9) || ElementalAvatarAbilityInBand(527, 9))
    {
        return false;
    }
    if (AvatarPetAbilityBit(528) != 16)
    {
        return false;
    }
    if (!IsDiabolosAbility(656) || !IsDiabolosAbility(671) || IsDiabolosAbility(655))
    {
        return false;
    }
    if (!IsCaitSithAbility(521) || IsCaitSithAbility(520) || IsCaitSithAbility(528))
    {
        return false;
    }
    if (!IsSirenAbility(960) || !IsSirenAbility(970) || IsSirenAbility(959))
    {
        return false;
    }
    if (SirenPetAbilityBit(960) != 0x1C0 || JugPetAbilityBit(520) != 8)
    {
        return false;
    }
    if (!ShouldCalculateBlueTraits(16, 1) || !ShouldCalculateBlueTraits(1, 16) || ShouldCalculateBlueTraits(1, 2))
    {
        return false;
    }
    const auto normalTraits = traitssourcehelpers::PlanFor({
        .mainJob   = 1,
        .subJob    = petabilitytablehelpers::JobBLU,
        .mainLevel = 99,
        .subLevel  = 49,
    });
    const auto monstrosityTraits = traitssourcehelpers::PlanFor({
        .mainJob             = 1,
        .subJob              = 2,
        .mainLevel           = 99,
        .subLevel            = 49,
        .hasMonstrosity      = true,
        .monstrosityMainJob  = petabilitytablehelpers::JobBLU,
        .monstrositySubJob   = 1,
        .monstrosityLevel    = 50,
    });
    const auto noBlueTraits = traitssourcehelpers::PlanFor({ .mainJob = 1, .subJob = 2 });
    if (normalTraits.mainJob != 1 || normalTraits.subJob != petabilitytablehelpers::JobBLU || normalTraits.mainLevel != 99 ||
        normalTraits.subLevel != 49 || !normalTraits.calculateBlueTraits || monstrosityTraits.mainJob != petabilitytablehelpers::JobBLU ||
        monstrosityTraits.subJob != 1 || monstrosityTraits.mainLevel != 50 || monstrosityTraits.subLevel != 50 ||
        !monstrosityTraits.calculateBlueTraits || noBlueTraits.calculateBlueTraits)
    {
        return false;
    }
    const auto normalTraitRoster = traitrosterhelpers::PlanFor(normalTraits);
    const auto monstrosityTraitRoster = traitrosterhelpers::PlanFor(monstrosityTraits);
    if (normalTraitRoster.addTraitSources != std::vector<traitrosterhelpers::Source>{
            { .job = 1, .level = 99 },
            { .job = petabilitytablehelpers::JobBLU, .level = 49 },
        } ||
        monstrosityTraitRoster.addTraitSources != std::vector<traitrosterhelpers::Source>{
            { .job = petabilitytablehelpers::JobBLU, .level = 50 },
            { .job = 1, .level = 50 },
        })
    {
        return false;
    }
    return true;
}
} // namespace

auto runPetAbilityTable1514SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "pet ability table 1514 self-test failed\n";
    }
    return ok;
}
