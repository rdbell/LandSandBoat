#include "test_claim_1525.h"

#include "map/claim_capacity.h"

#include <iostream>

namespace
{
using claimhelpers::BattleTargetAllowsClaim;
using claimhelpers::ClaimMobEarlyAction;
using claimhelpers::ClaimOwnershipPath;
using claimhelpers::ClaimType;
using claimhelpers::ClassifyClaimMobEarly;
using claimhelpers::ClassifyClaimOwnership;
using claimhelpers::DirtyExpMember;
using claimhelpers::DirtyExpMerge;
using claimhelpers::HasClaim;
using claimhelpers::HighestEnmityAllowsClaim;
using claimhelpers::ResolveHighestEnmityIsTrustPromote;
using claimhelpers::RelinquishPassCandidate;
using claimhelpers::ShouldAssignPClaimedMob;
using claimhelpers::ShouldDirtyExpOnClaim;
using claimhelpers::ShouldRelinquishOwnedMob;
using claimhelpers::ShouldUnclaimPreviousMob;

auto Check() -> bool
{
    if (!HasClaim(10, 10, false) || HasClaim(10, 20, false) || !HasClaim(10, 20, true))
    {
        return false;
    }

    DirtyExpMember members[4] = {
        { true, true, 50 },
        { true, true, 55 },
        { true, false, 75 },
        { false, true, 75 },
    };
    std::uint8_t hiP = 0;
    std::uint8_t hiL = 0;
    DirtyExpMerge(2, 40, members, 4, hiP, hiL);
    if (hiP != 2 || hiL != 55)
    {
        return false;
    }

    if (!ShouldRelinquishOwnedMob(true, true, 7, 7) || ShouldRelinquishOwnedMob(true, false, 7, 7))
    {
        return false;
    }
    if (!RelinquishPassCandidate(false, false, true, true, true, false) ||
        RelinquishPassCandidate(true, false, true, true, true, false))
    {
        return false;
    }

    if (ClassifyClaimMobEarly(false, false, true, false, ClaimType::Exclusive, false) != ClaimMobEarlyAction::SkipNonMob)
    {
        return false;
    }
    if (ClassifyClaimMobEarly(true, true, true, false, ClaimType::Exclusive, false) != ClaimMobEarlyAction::SkipAllied)
    {
        return false;
    }
    if (ClassifyClaimMobEarly(true, false, false, false, ClaimType::Exclusive, false) != ClaimMobEarlyAction::SkipNoPCMaster)
    {
        return false;
    }
    if (ClassifyClaimMobEarly(true, false, true, false, ClaimType::Unclaimable, true) != ClaimMobEarlyAction::SkipUnclaimable)
    {
        return false;
    }
    if (ClassifyClaimMobEarly(true, false, true, false, ClaimType::Unclaimable, false) != ClaimMobEarlyAction::Proceed)
    {
        return false;
    }

    if (ShouldDirtyExpOnClaim(true) || !ShouldDirtyExpOnClaim(false))
    {
        return false;
    }
    if (!BattleTargetAllowsClaim(false, false, false, false) || BattleTargetAllowsClaim(true, false, true, false))
    {
        return false;
    }
    if (ClassifyClaimOwnership(true, false, true, false) != ClaimOwnershipPath::AllianceUpdate)
    {
        return false;
    }
    if (ClassifyClaimOwnership(true, true, true, false) != ClaimOwnershipPath::CFHBlocked)
    {
        return false;
    }
    if (ClassifyClaimOwnership(true, false, false, true) != ClaimOwnershipPath::KillingBlowUnclaimed)
    {
        return false;
    }
    if (!ShouldUnclaimPreviousMob(true, true, false, true, 9, 9) || ShouldAssignPClaimedMob(false))
    {
        return false;
    }
    if (!HighestEnmityAllowsClaim(false, false, false) || HighestEnmityAllowsClaim(true, false, false))
    {
        return false;
    }
    if (!ResolveHighestEnmityIsTrustPromote(true) || ResolveHighestEnmityIsTrustPromote(false))
    {
        return false;
    }
    return true;
}
} // namespace

auto runClaim1525SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "claim_1525 self-tests failed\n";
        return false;
    }
    return true;
}
