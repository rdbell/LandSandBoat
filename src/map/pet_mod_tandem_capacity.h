#pragma once

#include <cstdint>

// Pure petutils CheckPetModType / IsTandemActive / ExtendCharm gates.
// Parity: internal/petutils (check_pet_mod, tandem, extend_charm; slice 1624).

namespace petmodtandemhelpers
{

// PetModType pins (modifier.h).
constexpr std::uint8_t PetModAll       = 0;
constexpr std::uint8_t PetModAvatar    = 1;
constexpr std::uint8_t PetModWyvern    = 2;
constexpr std::uint8_t PetModAutomaton = 3;
constexpr std::uint8_t PetModHarlequin = 4;
constexpr std::uint8_t PetModValoredge = 5;
constexpr std::uint8_t PetModSharpshot = 6;
constexpr std::uint8_t PetModStormwaker = 7;
constexpr std::uint8_t PetModLuopan    = 8;

// PET_TYPE pins (pet_entity.h).
constexpr std::uint8_t PetTypeAvatar    = 0;
constexpr std::uint8_t PetTypeWyvern    = 1;
constexpr std::uint8_t PetTypeJugPet    = 2;
constexpr std::uint8_t PetTypeCharmed   = 3;
constexpr std::uint8_t PetTypeAutomaton = 4;
constexpr std::uint8_t PetTypeFellow    = 5;
constexpr std::uint8_t PetTypeChocobo   = 6;
constexpr std::uint8_t PetTypeLuopan    = 7;

// (uint16)petmod + 28 == (uint16)frame for frame-specific automaton mods.
constexpr std::uint16_t AutomatonFramePetModOffset = 28;

// Pure CheckPetModType once entity shape / type / frame are resolved.
inline auto CheckPetModType(const std::uint8_t petmod, const bool isPetEntity, const std::uint8_t petType, const std::uint16_t automatonFrame) -> bool
{
    if (petmod == PetModAll)
    {
        return true;
    }
    if (!isPetEntity)
    {
        return true;
    }
    switch (petmod)
    {
        case PetModAvatar:
            return petType == PetTypeAvatar;
        case PetModWyvern:
            return petType == PetTypeWyvern;
        case PetModAutomaton:
        case PetModHarlequin:
        case PetModValoredge:
        case PetModSharpshot:
        case PetModStormwaker:
            if (petType != PetTypeAutomaton)
            {
                return false;
            }
            if (petmod == PetModAutomaton)
            {
                return true;
            }
            return static_cast<std::uint16_t>(petmod) + AutomatonFramePetModOffset == automatonFrame;
        case PetModLuopan:
            return petType == PetTypeLuopan;
        default:
            return false;
    }
}

// Pure HasTandemPartner branch.
inline auto HasTandemPartner(const bool isPC, const bool hasPet, const bool hasMaster, const bool masterIsPC) -> bool
{
    if (isPC)
    {
        return hasPet;
    }
    return hasMaster && masterIsPC;
}

// Pure IsTandemActive once partner presence and engagement/target injects are ready.
inline auto IsTandemActive(const bool hasPartner, const bool partnerEngaged, const bool partnerHasTarget, const std::uint16_t partnerTargetID,
                           const std::uint16_t attackerTargetID) -> bool
{
    if (!hasPartner)
    {
        return false;
    }
    return partnerEngaged && partnerHasTarget && partnerTargetID == attackerTargetID;
}

// Combined convenience matching Go IsTandemActiveValues.
inline auto IsTandemActiveValues(const bool isPC, const bool hasPet, const bool hasMaster, const bool masterIsPC, const bool partnerEngaged,
                                 const bool partnerHasTarget, const std::uint16_t partnerTargetID, const std::uint16_t attackerTargetID) -> bool
{
    return IsTandemActive(HasTandemPartner(isPC, hasPet, hasMaster, masterIsPC), partnerEngaged, partnerHasTarget, partnerTargetID, attackerTargetID);
}

// ExtendCharm: only charmed mobs.
inline auto CanExtendCharm(const bool isMob, const bool isCharmed) -> bool
{
    return isMob && isCharmed;
}

// ExtendCharm range sanity: min <= max && max != 0.
inline auto CharmSecondsRangeValid(const std::uint16_t minSeconds, const std::uint16_t maxSeconds) -> bool
{
    return minSeconds <= maxSeconds && maxSeconds != 0;
}

} // namespace petmodtandemhelpers
