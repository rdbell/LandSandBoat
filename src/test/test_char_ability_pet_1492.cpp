#include "test_char_ability_pet_1492.h"

#include "map/char_ability_pet_capacity.h"

#include <iostream>

namespace
{
using charabilitypethelpers::ActionPacketTargetID;
using charabilitypethelpers::PetSkillTargetID;

auto Check() -> bool
{
    if (ActionPacketTargetID(true, 100, 200) != 100 || ActionPacketTargetID(false, 100, 200) != 200)
    {
        return false;
    }

    // Non-jug always uses ability target.
    if (PetSkillTargetID(false, true, 11, 22, 33) != 11 || PetSkillTargetID(false, false, 11, 22, 33) != 11)
    {
        return false;
    }

    // Jug + enemy skill → battle target.
    if (PetSkillTargetID(true, true, 11, 22, 33) != 22)
    {
        return false;
    }

    // Jug + non-enemy skill → pet self.
    if (PetSkillTargetID(true, false, 11, 22, 33) != 33)
    {
        return false;
    }

    return true;
}
} // namespace

auto runCharAbilityPet1492SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char ability pet 1492 self-test failed\n";
    }
    return ok;
}
