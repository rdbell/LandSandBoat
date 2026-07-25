#include "test_pet_command_mutation_6850.h"

#include "map/entities/char_entity.h"
#include "map/utils/charutils.h"

#include <cstdint>
#include <iostream>

namespace
{
auto Check() -> bool
{
    auto character = CCharEntity{};

    constexpr auto firstID      = std::uint16_t{ 0 };
    constexpr auto lastID       = std::uint16_t{ sizeof(character.m_PetCommands) * 8 - 1 };
    constexpr auto outOfRangeID = std::uint16_t{ sizeof(character.m_PetCommands) * 8 };

    if (charutils::hasPetAbility(&character, firstID) != 0 ||
        charutils::addPetAbility(&character, firstID) != 1 ||
        charutils::hasPetAbility(&character, firstID) == 0 ||
        charutils::addPetAbility(&character, firstID) != 0 ||
        charutils::delPetAbility(&character, firstID) != 1 ||
        charutils::hasPetAbility(&character, firstID) != 0 ||
        charutils::delPetAbility(&character, firstID) != 0)
    {
        return false;
    }

    return charutils::addPetAbility(&character, lastID) == 1 &&
           charutils::hasPetAbility(&character, lastID) != 0 &&
           charutils::delPetAbility(&character, lastID) == 1 &&
           charutils::addPetAbility(&character, outOfRangeID) == 0 &&
           charutils::delPetAbility(&character, outOfRangeID) == 0;
}
} // namespace

auto runPetCommandMutation6850SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "pet command mutation 6850 self-test failed\n";
    }
    return ok;
}
