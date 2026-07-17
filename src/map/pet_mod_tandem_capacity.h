#pragma once

#include <cstdint>

// Pure petutils CheckPetModType / IsTandemActive / ExtendCharm gates.
// Parity: internal/petutils (check_pet_mod, tandem, extend_charm; slice 1624).
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1624: CheckPetModType / IsTandemActive / ExtendCharm production wire
//           (CanExtendCharm / CharmSecondsRangeValid residual pure)
//   - 2922: ShouldExtendCharm residual dual-wire suite
//           (CanExtendCharm && CharmSecondsRangeValid combined gate)
//   - 3220: ShouldExtendCharm dedicated dual-wire (extend_charm.go;
//           expand residual 2922 / pure 1624)
//
// Dual-wire index:
//   - 2922: ShouldExtendCharm residual dual-wire suite
//   - 3220: ShouldExtendCharm = isMob && isCharmed && minSeconds <= maxSeconds
//           && maxSeconds != 0
//
// Production host: petutils::ExtendCharm (petutils.cpp) still uses two
// sequential 1624 early returns (CanExtendCharm then CharmSecondsRangeValid);
// combined free function is the dual-wire surface for tests / future inject.
// Go dual-wire: petutils.ShouldExtendCharm
// (internal/petutils/extend_charm.go).
// Residual dual-wire suite: 2922 (test_petutils_extend_charm_2922).
// Dedicated dual-wire suite: 3220 (test_petutils_extend_charm_3220).

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

// ---------------------------------------------------------------------------
// Slice 3220 — ExtendCharm combined early-return gate
// (dedicated expand residual 2922)
// ---------------------------------------------------------------------------

// ShouldExtendCharm combines both ExtendCharm early-return gates:
// CanExtendCharm && CharmSecondsRangeValid.
//
// Formula (slice 3220 dedicated dual-wire; residual expand 2922 / pure 1624 —
// formula unchanged):
//   isMob && isCharmed && minSeconds <= maxSeconds && maxSeconds != 0
//
// isMob       — host-injected PPet->objtype == TYPE_MOB
// isCharmed   — host-injected PPet->isCharmed
// minSeconds  — host-injected ExtendCharm minSeconds
// maxSeconds  — host-injected ExtendCharm maxSeconds
// true  → host may draw RNG and write charmTime
// false → early return (not charmed mob and/or invalid range)
//
// Dual-wire of Go petutils.ShouldExtendCharm.
// Call site: petutils::ExtendCharm — residual production still uses two
// sequential 1624 helpers; combined free function is dual-wire surface for
// tests / future single-gate inject before xirand / charmTime writeback.
// Prior pure port: slice 1624 (CanExtendCharm / CharmSecondsRangeValid).
// Residual dual-wire suite: 2922 / test_petutils_extend_charm_2922.
// Dedicated dual-wire suite: 3220 / test_petutils_extend_charm_3220.
// Host still owns RNG and charmTime.
// Parity: internal/petutils.ShouldExtendCharm.
inline auto ShouldExtendCharm(const bool isMob, const bool isCharmed, const std::uint16_t minSeconds,
                              const std::uint16_t maxSeconds) -> bool
{
    return CanExtendCharm(isMob, isCharmed) && CharmSecondsRangeValid(minSeconds, maxSeconds);
}

} // namespace petmodtandemhelpers
