#include "test_party_trust_disband_6874.h"

#include "map/char_party_trust_disband.h"

#include <iostream>

auto runPartyTrustDisband6874SelfTests() -> bool
{
    const bool ok = partytrustdisbandhelpers::MakePlan(false, true, true) == partytrustdisbandhelpers::Plan{} &&
                    partytrustdisbandhelpers::MakePlan(true, false, true) == partytrustdisbandhelpers::Plan{} &&
                    partytrustdisbandhelpers::MakePlan(true, true, false) == partytrustdisbandhelpers::Plan{} &&
                    partytrustdisbandhelpers::MakePlan(true, true, true) == partytrustdisbandhelpers::Plan{ .checkMemberCountAcrossProcesses = true } &&
                    !partytrustdisbandhelpers::ShouldDisband(0) &&
                    partytrustdisbandhelpers::ShouldDisband(1) &&
                    !partytrustdisbandhelpers::ShouldDisband(2);
    if (!ok)
    {
        std::cerr << "party trust disband 6874 self-test failed\n";
    }
    return ok;
}
