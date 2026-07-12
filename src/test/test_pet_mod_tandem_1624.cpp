#include "test_pet_mod_tandem_1624.h"

#include "map/pet_mod_tandem_capacity.h"

#include <iostream>

namespace
{
using namespace petmodtandemhelpers;

auto Check() -> bool
{
    // CheckPetModType
    if (!CheckPetModType(PetModAll, true, PetTypeJugPet, 0) || !CheckPetModType(PetModAll, false, 0, 0))
    {
        return false;
    }
    if (!CheckPetModType(PetModAvatar, false, 0, 0)) // non-pet accepts any
    {
        return false;
    }
    if (!CheckPetModType(PetModAvatar, true, PetTypeAvatar, 0) || CheckPetModType(PetModAvatar, true, PetTypeWyvern, 0))
    {
        return false;
    }
    if (!CheckPetModType(PetModAutomaton, true, PetTypeAutomaton, 0x20))
    {
        return false;
    }
    // Harlequin petmod 4 + 28 = 0x20
    if (!CheckPetModType(PetModHarlequin, true, PetTypeAutomaton, 0x20) || CheckPetModType(PetModHarlequin, true, PetTypeAutomaton, 0x21))
    {
        return false;
    }
    if (CheckPetModType(PetModHarlequin, true, PetTypeAvatar, 0x20))
    {
        return false;
    }
    if (!CheckPetModType(PetModLuopan, true, PetTypeLuopan, 0) || CheckPetModType(PetModLuopan, true, PetTypeAvatar, 0))
    {
        return false;
    }

    // Tandem
    if (!HasTandemPartner(true, true, false, false) || HasTandemPartner(true, false, true, true))
    {
        return false;
    }
    if (!HasTandemPartner(false, false, true, true) || HasTandemPartner(false, false, true, false))
    {
        return false;
    }
    if (!IsTandemActive(true, true, true, 7, 7) || IsTandemActive(true, true, true, 7, 8))
    {
        return false;
    }
    if (IsTandemActive(false, true, true, 7, 7))
    {
        return false;
    }
    if (!IsTandemActiveValues(true, true, false, false, true, true, 5, 5))
    {
        return false;
    }
    if (IsTandemActiveValues(true, false, true, true, true, true, 5, 5))
    {
        return false;
    }

    // ExtendCharm
    if (!CanExtendCharm(true, true) || CanExtendCharm(true, false) || CanExtendCharm(false, true))
    {
        return false;
    }
    if (!CharmSecondsRangeValid(1, 5) || !CharmSecondsRangeValid(3, 3))
    {
        return false;
    }
    if (CharmSecondsRangeValid(5, 1) || CharmSecondsRangeValid(0, 0) || CharmSecondsRangeValid(1, 0))
    {
        return false;
    }

    return true;
}
} // namespace

auto runPetModTandem1624SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "pet_mod_tandem_1624 self-tests failed\n";
        return false;
    }
    return true;
}
