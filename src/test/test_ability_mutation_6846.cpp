#include "test_ability_mutation_6846.h"

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
    constexpr auto lastID       = std::uint16_t{ sizeof(character.m_Abilities) * 8 - 1 };
    constexpr auto outOfRangeID = std::uint16_t{ sizeof(character.m_Abilities) * 8 };

    if (charutils::hasAbility(&character, firstID) != 0 ||
        charutils::addAbility(&character, firstID) != 1 ||
        charutils::hasAbility(&character, firstID) == 0 ||
        charutils::addAbility(&character, firstID) != 0 ||
        charutils::delAbility(&character, firstID) != 1 ||
        charutils::hasAbility(&character, firstID) != 0 ||
        charutils::delAbility(&character, firstID) != 0)
    {
        return false;
    }

    if (charutils::addAbility(&character, lastID) != 1 ||
        charutils::hasAbility(&character, lastID) == 0 ||
        charutils::delAbility(&character, lastID) != 1 ||
        charutils::addAbility(&character, outOfRangeID) != 0 ||
        charutils::delAbility(&character, outOfRangeID) != 0)
    {
        return false;
    }

    return true;
}
} // namespace

auto runAbilityMutation6846SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "ability mutation 6846 self-test failed\n";
    }
    return ok;
}
