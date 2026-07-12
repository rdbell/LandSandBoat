#include "test_pet_detach_1626.h"

#include "map/pet_detach_capacity.h"

#include <iostream>

namespace
{
using namespace petdetachhelpers;

auto Check() -> bool
{
    if (ValidateDetachMaster(false, true) != DetachReject::MasterNull)
    {
        return false;
    }
    if (ValidateDetachMaster(true, false) != DetachReject::PetNull)
    {
        return false;
    }
    if (ValidateDetachMaster(true, true) != DetachReject::OK)
    {
        return false;
    }
    if (ValidateDetachPet(true, true, false) != DetachReject::MasterNotPC)
    {
        return false;
    }
    if (ValidateDetachPet(true, true, true) != DetachReject::OK)
    {
        return false;
    }

    if (ClassifyDetachPet(EntityTypeMOB) != DetachKind::CharmedMob)
    {
        return false;
    }
    if (ClassifyDetachPet(EntityTypePET) != DetachKind::OwnedPet)
    {
        return false;
    }
    if (ClassifyDetachPet(EntityTypePC) != DetachKind::None)
    {
        return false;
    }

    {
        const auto p = PlanCharmedMobAlive(true, false, false, true);
        if (!p.disengage || !p.withinEnmityRange || !p.giveExp || p.clearEnmityLeaveOrDead)
        {
            return false;
        }
    }
    {
        const auto p = PlanCharmedMobAlive(false, true, false, false);
        if (p.withinEnmityRange || !p.clearEnmityLeaveOrDead || p.giveExp)
        {
            return false;
        }
    }
    {
        const auto p = PlanCharmedMobAlive(false, false, true, false);
        if (!p.clearEnmityLeaveOrDead)
        {
            return false;
        }
    }

    if (!ShouldDieOwnedPet(false) || ShouldDieOwnedPet(true))
    {
        return false;
    }
    if (!ShouldClearAvatarPerpetuation(PetTypeAvatar) || ShouldClearAvatarPerpetuation(1))
    {
        return false;
    }
    return true;
}
} // namespace

auto runPetDetach1626SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "pet_detach_1626 self-tests failed\n";
        return false;
    }
    return true;
}
