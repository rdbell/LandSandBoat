#include "test_pet_spawn_1625.h"

#include "map/pet_spawn_capacity.h"

#include <iostream>

namespace
{
using namespace petspawnhelpers;

auto Check() -> bool
{
    if (!ShouldAddPetTraits(0) || ShouldAddPetTraits(PetTypeAutomaton))
    {
        return false;
    }
    if (!SpawnPetBlocked(true) || SpawnPetBlocked(false))
    {
        return false;
    }
    if (!LoadPetIDValid(0) || !LoadPetIDValid(127) || LoadPetIDValid(128) || LoadPetIDValid(MaxPetID))
    {
        return false;
    }
    if (!ShouldSetupPetWithMaster(true) || ShouldSetupPetWithMaster(false))
    {
        return false;
    }
    if (!UseNormalSpawnAnimation(true) || UseNormalSpawnAnimation(false))
    {
        return false;
    }
    if (!ShouldLoadZoningInfo(true) || ShouldLoadZoningInfo(false))
    {
        return false;
    }
    {
        const auto ids = ActivePropagatedStatuses(true, false, true);
        if (ids.size() != 2 || ids[0] != StatusDebilitation || ids[1] != StatusImpairment)
        {
            return false;
        }
    }
    if (!ActivePropagatedStatuses(false, false, false).empty())
    {
        return false;
    }
    {
        const auto ids = ActivePropagatedStatuses(true, true, true);
        if (ids.size() != 3 || ids[1] != StatusOmerta)
        {
            return false;
        }
    }
    return true;
}
} // namespace

auto runPetSpawn1625SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "pet_spawn_1625 self-tests failed\n";
        return false;
    }
    return true;
}
