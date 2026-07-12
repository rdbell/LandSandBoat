#include "test_pet_engage_1627.h"

#include "map/pet_engage_capacity.h"

#include <iostream>

namespace
{
using namespace petengagehelpers;

auto Check() -> bool
{
    if (!CanAttackTarget(true, true, true) || CanAttackTarget(false, true, true) || CanAttackTarget(true, false, true) || CanAttackTarget(true, true, false))
    {
        return false;
    }
    if (!ShouldPetEngage(false) || ShouldPetEngage(true))
    {
        return false;
    }
    if (!CanRetreatToMaster(true, true) || CanRetreatToMaster(false, true) || CanRetreatToMaster(true, false))
    {
        return false;
    }
    if (!ShouldPetDisengage(false) || ShouldPetDisengage(true))
    {
        return false;
    }
    return true;
}
} // namespace

auto runPetEngage1627SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "pet_engage_1627 self-tests failed\n";
        return false;
    }
    return true;
}
