#pragma once

#include <cstdint>

// Pure removeCharFromZone decision tree, orchestrion/mannequin mog-safe scans,
// and loadDeathTimestamp remaining-death math from charutils.

namespace zoneouthelpers
{

// STATUS / ANIMATION pins from base_entity.h.
constexpr std::uint8_t StatusDisappear = 2;
constexpr std::uint8_t StatusShutdown  = 20;
constexpr std::uint8_t AnimationNone   = 0;
constexpr std::uint8_t AnimationAttack = 1;

// Session shuttingDown values written by removeCharFromZone.
constexpr std::uint8_t ShuttingDownLogout = 1; // STATUS_SHUTDOWN path
constexpr std::uint8_t ShuttingDownZoning = 2; // normal zone change path

// char_stats.zoning column: 0 on logout, 1 on zone change.
constexpr std::uint8_t CharStatsZoningLogout = 0;
constexpr std::uint8_t CharStatsZoningActive = 1;

// Orchestrion furniture item ID scanned in isOrchestrionPlaced.
constexpr std::uint16_t OrchestrionItemID = 426;

// Mogsafe container IDs.
constexpr std::uint8_t LocMogsafe  = 1;
constexpr std::uint8_t LocMogsafe2 = 9;

// Death duration pin (CCharEntity::death_duration = 60min).
constexpr std::int64_t DeathDurationSeconds = 60 * 60;

// --- Session / animation ---

// ShouldMarkBlowfishPendingZone mirrors PSession non-null.
constexpr auto ShouldMarkBlowfishPendingZone(const bool hasSession) -> bool
{
    return hasSession;
}

// ShouldNotifyNominateOnLeave mirrors loc.zone != nullptr.
constexpr auto ShouldNotifyNominateOnLeave(const bool hasZone) -> bool
{
    return hasZone;
}

// ShouldClearAttackAnimation mirrors animation == ANIMATION_ATTACK.
constexpr auto ShouldClearAttackAnimation(const std::uint8_t animation) -> bool
{
    return animation == AnimationAttack;
}

// ShouldClearTrusts mirrors !PTrusts.empty().
constexpr auto ShouldClearTrusts(const bool hasTrusts) -> bool
{
    return hasTrusts;
}

// IsShutdownLogout mirrors status == STATUS_TYPE::SHUTDOWN.
constexpr auto IsShutdownLogout(const std::uint8_t status) -> bool
{
    return status == StatusShutdown;
}

// --- Party/alliance leave classification on logout ---

enum class ZoneOutPartyAction : std::uint8_t
{
    None = 0,
    DissolveAlliance,   // leader, only member, alliance only one party
    RemovePartyFromAlliance, // leader, only member, multi-party alliance
    RemoveMember,       // leader with others, or non-leader, or normal party
};

// ClassifyZoneOutPartyLeave mirrors the nested if tree under status==SHUTDOWN && PParty.
constexpr auto ClassifyZoneOutPartyLeave(const bool hasParty,
                                         const bool hasAlliance,
                                         const bool isLeader,
                                         const bool partyHasOnlyOneMember,
                                         const bool allianceHasOnlyOneParty) -> ZoneOutPartyAction
{
    if (!hasParty)
    {
        return ZoneOutPartyAction::None;
    }
    if (!hasAlliance)
    {
        return ZoneOutPartyAction::RemoveMember;
    }
    if (!isLeader)
    {
        return ZoneOutPartyAction::RemoveMember;
    }
    // Leader in alliance
    if (partyHasOnlyOneMember)
    {
        if (allianceHasOnlyOneParty)
        {
            return ZoneOutPartyAction::DissolveAlliance;
        }
        return ZoneOutPartyAction::RemovePartyFromAlliance;
    }
    return ZoneOutPartyAction::RemoveMember; // pass lead via RemoveMember
}

// ShouldSetPetZoningInfo mirrors shouldPetPersistThroughZoning() true branch.
// Host injects already-resolved shouldPetPersistThroughZoning().
constexpr auto ShouldSetPetZoningInfo(const bool shouldPetPersist) -> bool
{
    return shouldPetPersist;
}

// SessionShuttingDownValue mirrors logout→1 else→2.
constexpr auto SessionShuttingDownValue(const bool isShutdownLogout) -> std::uint8_t
{
    return isShutdownLogout ? ShuttingDownLogout : ShuttingDownZoning;
}

// CharStatsZoningValue mirrors logout→0 else→1.
constexpr auto CharStatsZoningValue(const bool isShutdownLogout) -> std::uint8_t
{
    return isShutdownLogout ? CharStatsZoningLogout : CharStatsZoningActive;
}

// ShouldDecreaseZoneCounter mirrors loc.zone != nullptr.
constexpr auto ShouldDecreaseZoneCounter(const bool hasZone) -> bool
{
    return hasZone;
}

// SaveStatusEffectsLogoutFlag mirrors shuttingDown == 1.
constexpr auto SaveStatusEffectsLogoutFlag(const std::uint8_t shuttingDown) -> bool
{
    return shuttingDown == ShuttingDownLogout;
}

// --- isOrchestrionPlaced pure furniture scan ---

// IsMogsafeContainer mirrors LOC_MOGSAFE or LOC_MOGSAFE2.
constexpr auto IsMogsafeContainer(const std::uint8_t containerID) -> bool
{
    return containerID == LocMogsafe || containerID == LocMogsafe2;
}

// IsOrchestrionFurniture mirrors furnishing, installed, ID==426.
constexpr auto IsOrchestrionFurniture(const bool isFurnishing, const bool isInstalled, const std::uint16_t itemID) -> bool
{
    return isFurnishing && isInstalled && itemID == OrchestrionItemID;
}

// IsValidMogsafeSlotIndex mirrors slotIndex in 1..size.
constexpr auto IsValidMogsafeSlotIndex(const std::int32_t slotIndex, const std::int32_t size) -> bool
{
    return slotIndex >= 1 && slotIndex <= size;
}

// --- updateMannequins pure gates ---

// IsInstalledMannequin mirrors furnishing, installed, isMannequin.
constexpr auto IsInstalledMannequin(const bool isFurnishing, const bool isInstalled, const bool isMannequin) -> bool
{
    return isFurnishing && isInstalled && isMannequin;
}

// ShouldWarnInvalidMannequinRace mirrors Race == 0.
constexpr auto ShouldWarnInvalidMannequinRace(const std::uint8_t race) -> bool
{
    return race == 0;
}

// --- loadDeathTimestamp pure ---

// ShouldApplyDeathTimestamp mirrors health.hp == 0 after loading death seconds.
constexpr auto ShouldApplyDeathTimestamp(const std::uint32_t currentHP) -> bool
{
    return currentHP == 0;
}

// RemainingDeathDurationSeconds mirrors death_duration - secondsSinceDeath (may be negative).
constexpr auto RemainingDeathDurationSeconds(const std::int64_t deathDurationSeconds, const std::uint32_t secondsSinceDeath) -> std::int64_t
{
    return deathDurationSeconds - static_cast<std::int64_t>(secondsSinceDeath);
}

// DeathTimeFromNowOffset is now - secondsSinceDeath (host applies timer arithmetic).
// Pure: seconds since death as duration to subtract from now.
constexpr auto DeathSecondsSinceDeath(const std::uint32_t secondsSinceDeath) -> std::uint32_t
{
    return secondsSinceDeath;
}

} // namespace zoneouthelpers
