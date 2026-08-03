#include "test_entity_action_1528.h"

#include "map/entity_action_capacity.h"

#include <iostream>

namespace
{
using entityactionhelpers::AbsorbedPhysDmgToMP;
using entityactionhelpers::AssistLockOn;
using entityactionhelpers::ClassifyAssistTarget;
using entityactionhelpers::ClassifyStandUp;
using entityactionhelpers::CoverGeometryOK;
using entityactionhelpers::CoverMemberEligible;
using entityactionhelpers::IsMagicCovered;
using entityactionhelpers::RangedAccuracyBonuses;
using entityactionhelpers::RangedAttackBonuses;
using entityactionhelpers::RangedDelayReduction;
using entityactionhelpers::ShouldApplyAbsorbedMP;
using entityactionhelpers::ShouldSearchCoverParty;
using entityactionhelpers::ShouldStandUpPet;
using entityactionhelpers::ShouldTurnTowardsTarget;
using entityactionhelpers::StandUpAction;

auto Check() -> bool
{
    if (ClassifyStandUp(false, 33) != StandUpAction::None)
    {
        return false;
    }
    if (ClassifyStandUp(true, 33) != StandUpAction::CancelHealing)
    {
        return false;
    }
    if (ClassifyStandUp(true, 47) != StandUpAction::LeaveSit || ClassifyStandUp(true, 63) != StandUpAction::LeaveSit ||
        ClassifyStandUp(true, 73) != StandUpAction::LeaveSit)
    {
        return false;
    }
    if (!ShouldStandUpPet(true, 1) || !ShouldStandUpPet(true, 4) || ShouldStandUpPet(true, 2))
    {
        return false;
    }

    if (ClassifyAssistTarget(false, true, true, true) != AssistLockOn::None)
    {
        return false;
    }
    if (ClassifyAssistTarget(true, true, true, true) != AssistLockOn::LockOnResolved)
    {
        return false;
    }
    if (ClassifyAssistTarget(true, true, true, false) != AssistLockOn::None)
    {
        return false;
    }
    if (ClassifyAssistTarget(true, false, true, false) != AssistLockOn::LockOnResolved)
    {
        return false;
    }

    if (ShouldTurnTowardsTarget(false, true, false, 0, false) ||
        ShouldTurnTowardsTarget(true, true, true, 0x400, false) || !ShouldTurnTowardsTarget(true, true, true, 0x400, true))
    {
        return false;
    }

    if (RangedDelayReduction(100, 50, false, 0) != 50 || RangedDelayReduction(100, 90, false, 0) != 30)
    {
        return false;
    }
    if (RangedDelayReduction(100, 50, true, 0) != 42)
    {
        return false;
    }
    if (RangedAttackBonuses(false, true, 20) != 0 || RangedAttackBonuses(true, true, 20) != 20)
    {
        return false;
    }
    if (RangedAccuracyBonuses(true, true, 15) != 15 || RangedAccuracyBonuses(true, false, 15) != 0)
    {
        return false;
    }

    if (!IsMagicCovered(true, 1) || IsMagicCovered(false, 1) || IsMagicCovered(true, 0))
    {
        return false;
    }
    if (!ShouldSearchCoverParty(true) || ShouldSearchCoverParty(false))
    {
        return false;
    }
    if (!CoverMemberEligible(true, true, true) || !CoverGeometryOK(5.f, 10.f, 0.5f, 8.f, true))
    {
        return false;
    }
    if (CoverGeometryOK(11.f, 10.f, 0.5f, 8.f, true))
    {
        return false;
    }

    if (AbsorbedPhysDmgToMP(100, true, 10, 5) != 15 || AbsorbedPhysDmgToMP(100, false, 10, 5) != 5)
    {
        return false;
    }
    if (!ShouldApplyAbsorbedMP(1) || ShouldApplyAbsorbedMP(0))
    {
        return false;
    }
    return true;
}
} // namespace

auto runEntityAction1528SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "entity_action_1528 self-tests failed\n";
        return false;
    }
    return true;
}
