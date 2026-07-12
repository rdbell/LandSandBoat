#pragma once

#include <cstdint>

// Pure BuildingCharAbilityTable and CheckAbilityAddtype policy from charutils.

namespace abilitytablehelpers
{

// Ability / job / addtype pins from ability.h and battle_entity.h.
constexpr std::uint16_t AbilityHealingRuby  = 512;
constexpr std::uint16_t AbilityPetCommands  = 55;
constexpr std::uint8_t  JobNon              = 0;

constexpr std::uint16_t AddTypeMerit       = 1;
constexpr std::uint16_t AddTypeAstralFlow  = 2;
constexpr std::uint16_t AddTypeMainOnly    = 4;
constexpr std::uint16_t AddTypeLearned     = 8;
constexpr std::uint16_t AddTypeLightArts   = 16;
constexpr std::uint16_t AddTypeDarkArts    = 32;
constexpr std::uint16_t AddTypeJugPet      = 64;
constexpr std::uint16_t AddTypeCharmPet    = 128;
constexpr std::uint16_t AddTypeAvatar      = 256;
constexpr std::uint16_t AddTypeAutomaton   = 512;

// PETID pins that grant no avatar abilities to master.
constexpr std::uint32_t PetIDAlexander = 17;
constexpr std::uint32_t PetIDOdin      = 18;
constexpr std::uint32_t PetIDAtomos    = 19;

// ShouldRejectNullChar mirrors PChar == nullptr.
constexpr auto ShouldRejectNullChar(const bool isNull) -> bool
{
    return isNull;
}

// ShouldStopMainAbilityLoop mirrors level < abilityLevel (ordered ability list).
constexpr auto ShouldStopMainAbilityLoop(const std::uint8_t mLevel, const std::uint8_t abilityLevel) -> bool
{
    return mLevel < abilityLevel;
}

// ShouldAddMainAbility mirrors id < HEALING_RUBY && id != PET_COMMANDS && addtypeOK.
constexpr auto ShouldAddMainAbility(const std::uint16_t abilityID, const bool addtypeOK) -> bool
{
    return abilityID < AbilityHealingRuby && abilityID != AbilityPetCommands && addtypeOK;
}

// ShouldSkipSubJobAbilities mirrors GetSJob() == JOB_NON.
constexpr auto ShouldSkipSubJobAbilities(const std::uint8_t sJob) -> bool
{
    return sJob == JobNon;
}

// ShouldStopSubAbilityLoop mirrors sLevel < abilityLevel.
constexpr auto ShouldStopSubAbilityLoop(const std::uint8_t sLevel, const std::uint8_t abilityLevel) -> bool
{
    return sLevel < abilityLevel;
}

// ShouldAddSubAbility mirrors level != 0 && id < HEALING_RUBY && id != PET_COMMANDS
// && addtypeOK && !(addType & MAIN_ONLY).
constexpr auto ShouldAddSubAbility(const std::uint8_t abilityLevel, const std::uint16_t abilityID, const std::uint16_t addType, const bool addtypeOK) -> bool
{
    if (abilityLevel == 0 || abilityID >= AbilityHealingRuby)
    {
        return false;
    }
    if (abilityID == AbilityPetCommands || !addtypeOK)
    {
        return false;
    }
    if ((addType & AddTypeMainOnly) != 0)
    {
        return false;
    }
    return true;
}

// MeritAddtypeOK: if MERIT flag, merit must exist and count > 0.
constexpr auto MeritAddtypeOK(const std::uint16_t addType, const bool meritExists, const bool meritCountPositive) -> bool
{
    if ((addType & AddTypeMerit) == 0)
    {
        return true;
    }
    return meritExists && meritCountPositive;
}

// AstralFlowAddtypeOK
constexpr auto AstralFlowAddtypeOK(const std::uint16_t addType, const bool hasAstralFlow) -> bool
{
    if ((addType & AddTypeAstralFlow) == 0)
    {
        return true;
    }
    return hasAstralFlow;
}

// LearnedAddtypeOK
constexpr auto LearnedAddtypeOK(const std::uint16_t addType, const bool hasLearned) -> bool
{
    if ((addType & AddTypeLearned) == 0)
    {
        return true;
    }
    return hasLearned;
}

// LightArtsAddtypeOK
constexpr auto LightArtsAddtypeOK(const std::uint16_t addType, const bool hasLightArts) -> bool
{
    if ((addType & AddTypeLightArts) == 0)
    {
        return true;
    }
    return hasLightArts;
}

// DarkArtsAddtypeOK
constexpr auto DarkArtsAddtypeOK(const std::uint16_t addType, const bool hasDarkArts) -> bool
{
    if ((addType & AddTypeDarkArts) == 0)
    {
        return true;
    }
    return hasDarkArts;
}

// JugCharmBothAddtypeOK: both JUG|CHARM bits set → pet is mob or jug pet.
constexpr auto JugCharmBothAddtypeOK(const std::uint16_t addType, const bool hasPet, const bool petIsMobOrJugPet) -> bool
{
    if ((addType & (AddTypeJugPet | AddTypeCharmPet)) != (AddTypeJugPet | AddTypeCharmPet))
    {
        return true;
    }
    return hasPet && petIsMobOrJugPet;
}

// JugOnlyAddtypeOK: only JUG bit of the pair → jug pet entity.
constexpr auto JugOnlyAddtypeOK(const std::uint16_t addType, const bool hasPet, const bool petIsJugPet) -> bool
{
    if ((addType & (AddTypeJugPet | AddTypeCharmPet)) != AddTypeJugPet)
    {
        return true;
    }
    return hasPet && petIsJugPet;
}

// CharmOnlyAddtypeOK: only CHARM bit of the pair → mob pet.
constexpr auto CharmOnlyAddtypeOK(const std::uint16_t addType, const bool hasPet, const bool petIsMob) -> bool
{
    if ((addType & (AddTypeJugPet | AddTypeCharmPet)) != AddTypeCharmPet)
    {
        return true;
    }
    return hasPet && petIsMob;
}

// AvatarAddtypeOK: avatar pet and not Alexander/Odin/Atomos.
constexpr auto AvatarAddtypeOK(const std::uint16_t addType, const bool hasPet, const bool petIsAvatar, const std::uint32_t petID) -> bool
{
    if ((addType & AddTypeAvatar) == 0)
    {
        return true;
    }
    if (!hasPet || !petIsAvatar)
    {
        return false;
    }
    if (petID == PetIDAlexander || petID == PetIDOdin || petID == PetIDAtomos)
    {
        return false;
    }
    return true;
}

// AutomatonAddtypeOK
constexpr auto AutomatonAddtypeOK(const std::uint16_t addType, const bool hasPet, const bool petIsAutomaton) -> bool
{
    if ((addType & AddTypeAutomaton) == 0)
    {
        return true;
    }
    return hasPet && petIsAutomaton;
}

// CheckAbilityAddtype combines sequential gates (hosts inject status/pet/merit facts).
constexpr auto CheckAbilityAddtype(const std::uint16_t addType,
                                   const bool meritExists,
                                   const bool meritCountPositive,
                                   const bool hasAstralFlow,
                                   const bool hasLearned,
                                   const bool hasLightArts,
                                   const bool hasDarkArts,
                                   const bool hasPet,
                                   const bool petIsMobOrJugPet,
                                   const bool petIsJugPet,
                                   const bool petIsMob,
                                   const bool petIsAvatar,
                                   const std::uint32_t petID,
                                   const bool petIsAutomaton) -> bool
{
    if (!MeritAddtypeOK(addType, meritExists, meritCountPositive))
    {
        return false;
    }
    if (!AstralFlowAddtypeOK(addType, hasAstralFlow))
    {
        return false;
    }
    if (!LearnedAddtypeOK(addType, hasLearned))
    {
        return false;
    }
    if (!LightArtsAddtypeOK(addType, hasLightArts))
    {
        return false;
    }
    if (!DarkArtsAddtypeOK(addType, hasDarkArts))
    {
        return false;
    }
    if (!JugCharmBothAddtypeOK(addType, hasPet, petIsMobOrJugPet))
    {
        return false;
    }
    if (!JugOnlyAddtypeOK(addType, hasPet, petIsJugPet))
    {
        return false;
    }
    if (!CharmOnlyAddtypeOK(addType, hasPet, petIsMob))
    {
        return false;
    }
    if (!AvatarAddtypeOK(addType, hasPet, petIsAvatar, petID))
    {
        return false;
    }
    if (!AutomatonAddtypeOK(addType, hasPet, petIsAutomaton))
    {
        return false;
    }
    return true;
}

// ChargeTimeAfterMerit mirrors chargeTime - meritSeconds (host supplies durations as seconds).
constexpr auto ChargeTimeAfterMerit(const std::int64_t chargeTimeSeconds, const std::int32_t meritValueSeconds) -> std::int64_t
{
    return chargeTimeSeconds - static_cast<std::int64_t>(meritValueSeconds);
}

// ShouldInitAbilityRecast mirrors !Has(RECAST_ABILITY, recastId).
constexpr auto ShouldInitAbilityRecast(const bool alreadyHasRecast) -> bool
{
    return !alreadyHasRecast;
}

} // namespace abilitytablehelpers
