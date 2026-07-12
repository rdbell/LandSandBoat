#pragma once

#include <algorithm>
#include <cstdint>

// Pure ClaimMob / DirtyExp / RelinquishClaim / HasClaim decision helpers.

namespace claimhelpers
{

constexpr float DirtyExpDistance = 100.0f;

enum class ClaimType : std::int16_t
{
    Exclusive    = 0,
    NonExclusive = 1,
    Unclaimable  = 2,
};

constexpr auto HasClaim(const std::uint32_t masterID, const std::uint32_t targetOwnerID, const bool allianceHasOwner) -> bool
{
    if (targetOwnerID == masterID)
    {
        return true;
    }
    return allianceHasOwner;
}

constexpr auto ResolveClaimMasterOK(const bool attackerIsPC, const bool hasMaster, const bool masterIsPC) -> bool
{
    if (attackerIsPC)
    {
        return true;
    }
    return hasMaster && masterIsPC;
}

struct DirtyExpMember
{
    bool          sameZone{ false };
    bool          inRange{ false };
    std::uint8_t  mLevel{ 0 };
};

inline auto DirtyExpMerge(const std::uint8_t existingHiParty,
                          const std::uint8_t existingHiPCLvl,
                          const DirtyExpMember* members,
                          const std::size_t     count,
                          std::uint8_t&        outHiParty,
                          std::uint8_t&        outHiPCLvl) -> void
{
    std::uint8_t pcinzone = 0;
    std::uint8_t maxLevel = 0;
    for (std::size_t i = 0; i < count; ++i)
    {
        if (members[i].sameZone && members[i].inRange)
        {
            maxLevel = std::max(maxLevel, members[i].mLevel);
            ++pcinzone;
        }
    }
    outHiParty = std::max(pcinzone, existingHiParty);
    outHiPCLvl = std::max(maxLevel, existingHiPCLvl);
}

constexpr auto RelinquishPassCandidate(const bool isSelf,
                                       const bool alreadyFound,
                                       const bool sameZone,
                                       const bool memberAlive,
                                       const bool memberClaimedMobIsNil,
                                       const bool memberClaimedMobIsThis) -> bool
{
    if (isSelf || alreadyFound || !sameZone || !memberAlive)
    {
        return false;
    }
    return memberClaimedMobIsNil || memberClaimedMobIsThis;
}

constexpr auto ShouldRelinquishOwnedMob(const bool          hasClaimedMob,
                                        const bool          mobAlive,
                                        const std::uint32_t ownerID,
                                        const std::uint32_t charID) -> bool
{
    return hasClaimedMob && mobAlive && ownerID == charID;
}

enum class ClaimMobEarlyAction : std::uint8_t
{
    Proceed = 0,
    SkipNonMob,
    SkipAllied,
    SkipNoPCMaster,
    SkipUnclaimable,
};

constexpr auto ClassifyClaimMobEarly(const bool       defenderIsMob,
                                     const bool       sameAllegiance,
                                     const bool       attackerIsPC,
                                     const bool       hasPCMaster,
                                     const ClaimType  claimType,
                                     const bool       afterEnmityTap) -> ClaimMobEarlyAction
{
    if (!defenderIsMob)
    {
        return ClaimMobEarlyAction::SkipNonMob;
    }
    if (sameAllegiance)
    {
        return ClaimMobEarlyAction::SkipAllied;
    }
    if (!attackerIsPC && !hasPCMaster)
    {
        return ClaimMobEarlyAction::SkipNoPCMaster;
    }
    if (afterEnmityTap && claimType == ClaimType::Unclaimable)
    {
        return ClaimMobEarlyAction::SkipUnclaimable;
    }
    return ClaimMobEarlyAction::Proceed;
}

constexpr auto ShouldDirtyExpOnClaim(const bool passing) -> bool
{
    return !passing;
}

constexpr auto ShouldUpdateEnmityOnClaim(const bool passing) -> bool
{
    return !passing;
}

enum class ClaimOwnershipPath : std::uint8_t
{
    SkipBattleTarget = 0,
    AllianceUpdate,
    KillingBlowUnclaimed,
    HighestEnmityScan,
    CFHBlocked,
};

constexpr auto BattleTargetAllowsClaim(const bool hasBattleTarget,
                                       const bool battleTargetIsDefender,
                                       const bool battleTargetIsClaimedMob,
                                       const bool defenderDead) -> bool
{
    return !hasBattleTarget || battleTargetIsDefender || !battleTargetIsClaimedMob || defenderDead;
}

constexpr auto ClassifyClaimOwnership(const bool battleTargetAllowsClaim,
                                      const bool callForHelp,
                                      const bool hasClaim,
                                      const bool defenderDead) -> ClaimOwnershipPath
{
    if (!battleTargetAllowsClaim)
    {
        return ClaimOwnershipPath::SkipBattleTarget;
    }
    if (callForHelp)
    {
        return ClaimOwnershipPath::CFHBlocked;
    }
    if (hasClaim)
    {
        return ClaimOwnershipPath::AllianceUpdate;
    }
    if (defenderDead)
    {
        return ClaimOwnershipPath::KillingBlowUnclaimed;
    }
    return ClaimOwnershipPath::HighestEnmityScan;
}

constexpr auto ShouldUnclaimPreviousMob(const bool          defenderAlive,
                                        const bool          hasClaimedMob,
                                        const bool          claimedIsDefender,
                                        const bool          claimedAlive,
                                        const std::uint32_t claimedOwnerID,
                                        const std::uint32_t attackerID) -> bool
{
    return defenderAlive && hasClaimedMob && !claimedIsDefender && claimedAlive && claimedOwnerID == attackerID;
}

constexpr auto ShouldAssignPClaimedMob(const bool defenderAlive) -> bool
{
    return defenderAlive;
}

constexpr auto HighestEnmityAllowsClaim(const bool hasHighest, const bool highestIsMember, const bool highestIsMemberPet) -> bool
{
    return !hasHighest || highestIsMember || highestIsMemberPet;
}

constexpr auto ResolveHighestEnmityIsTrustPromote(const bool highestIsTrust) -> bool
{
    return highestIsTrust;
}

} // namespace claimhelpers
