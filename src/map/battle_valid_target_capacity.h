#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

namespace battlevalidtargethelpers
{

// ALLEGIANCE_TYPE pins from base_entity.h.
constexpr uint8 AllegianceMob      = 0;
constexpr uint8 AllegiancePlayer   = 1;
constexpr uint8 AllegianceSanDoria = 2;
constexpr uint8 AllegianceBastok   = 3;
constexpr uint8 AllegianceWindurst = 4;
constexpr uint8 AllegianceWyverns  = 5;
constexpr uint8 AllegianceGriffons = 6;

// TARGETTYPE pins from battle_entity.h.
constexpr uint16 TargetSelf  = 0x0001;
constexpr uint16 TargetEnemy = 0x0004;

// PET_TYPE::AUTOMATON pin.
constexpr uint8 PetTypeAutomaton = 4;

// TYPE_PET pin from base_entity.h ENTITYTYPE.
constexpr uint8 TypePet = 0x08;

// IsTeamPVPAllegiance mirrors allegiance >= WYVERNS for both parties.
inline auto IsTeamPVPAllegiance(const uint8 allegiance) -> bool
{
    return allegiance >= AllegianceWyverns;
}

// IsNationPVPAllegiance mirrors SAN_DORIA..WINDURST inclusive.
inline auto IsNationPVPAllegiance(const uint8 allegiance) -> bool
{
    return allegiance >= AllegianceSanDoria && allegiance <= AllegianceWindurst;
}

// IsPVEAllegiance mirrors allegiance <= PLAYER.
inline auto IsPVEAllegiance(const uint8 allegiance) -> bool
{
    return allegiance <= AllegiancePlayer;
}

// ResolveEnemyValidTarget mirrors the TARGET_ENEMY branch when !isDead.
// skipAllegianceCheck is MOBMOD_SKIP_ALLEGIANCE_CHECK == 1 on a mob initiator.
// initiatorIsMob gates the special self-heal path.
inline auto ResolveEnemyValidTarget(
    const uint8 selfAllegiance,
    const uint8 initiatorAllegiance,
    const bool initiatorIsMob,
    const bool skipAllegianceCheck) -> bool
{
    // Teams PVP
    if (IsTeamPVPAllegiance(selfAllegiance) && IsTeamPVPAllegiance(initiatorAllegiance))
    {
        return selfAllegiance != initiatorAllegiance;
    }

    // Nation PVP
    if (IsNationPVPAllegiance(selfAllegiance) && IsNationPVPAllegiance(initiatorAllegiance))
    {
        return selfAllegiance != initiatorAllegiance;
    }

    // PVE
    if (IsPVEAllegiance(selfAllegiance) && IsPVEAllegiance(initiatorAllegiance))
    {
        if (selfAllegiance != initiatorAllegiance)
        {
            return true;
        }
        // same allegiance: special mob mod for self-target enemy spells
        return initiatorIsMob && skipAllegianceCheck;
    }

    return false;
}

// HasEnemyTargetFlag mirrors (targetFlags & TARGET_ENEMY).
inline auto HasEnemyTargetFlag(const uint16 targetFlags) -> bool
{
    return (targetFlags & TargetEnemy) != 0;
}

// HasSelfTargetFlag mirrors (targetFlags & TARGET_SELF).
inline auto HasSelfTargetFlag(const uint16 targetFlags) -> bool
{
    return (targetFlags & TargetSelf) != 0;
}

// ResolveSelfValidTarget mirrors the TARGET_SELF fallback.
// isSelf is this == PInitiator; isAutomatonMaster is pet automaton targeting its master.
inline auto ResolveSelfValidTarget(const bool isSelf, const bool isAutomatonMaster) -> bool
{
    return isSelf || isAutomatonMaster;
}

// IsAutomatonMasterTarget mirrors TYPE_PET automaton initiator with this == master.
inline auto IsAutomatonMasterTarget(
    const bool initiatorIsPet,
    const uint8 petType,
    const bool selfIsMaster) -> bool
{
    return initiatorIsPet && petType == PetTypeAutomaton && selfIsMaster;
}

// ResolveValidTarget is the full pure decision tree for CBattleEntity::ValidTarget.
// When enemy flag is set and target is dead, enemy path fails (falls through to self).
inline auto ResolveValidTarget(
    const uint16 targetFlags,
    const bool isDead,
    const uint8 selfAllegiance,
    const uint8 initiatorAllegiance,
    const bool initiatorIsMob,
    const bool skipAllegianceCheck,
    const bool isSelf,
    const bool isAutomatonMaster) -> bool
{
    if (HasEnemyTargetFlag(targetFlags) && !isDead)
    {
        return ResolveEnemyValidTarget(
            selfAllegiance,
            initiatorAllegiance,
            initiatorIsMob,
            skipAllegianceCheck);
    }

    if (HasSelfTargetFlag(targetFlags))
    {
        return ResolveSelfValidTarget(isSelf, isAutomatonMaster);
    }
    return false;
}

} // namespace battlevalidtargethelpers
