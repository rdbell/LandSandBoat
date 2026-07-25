#include "test_learned_ability_mutation_6845.h"

#include "map/entities/char_entity.h"
#include "map/utils/charutils.h"

#include <cstdint>
#include <iostream>

namespace
{
auto Check() -> bool
{
    auto character = CCharEntity{};

    constexpr auto firstID = std::uint16_t{ 0 };
    constexpr auto lastID  = std::uint16_t{ sizeof(character.m_LearnedAbilities) * 8 - 1 };
    constexpr auto outOfRangeID = std::uint16_t{ sizeof(character.m_LearnedAbilities) * 8 };

    if (charutils::hasLearnedAbility(&character, firstID) != 0 ||
        charutils::addLearnedAbility(&character, firstID) != 1 ||
        charutils::hasLearnedAbility(&character, firstID) == 0 ||
        charutils::addLearnedAbility(&character, firstID) != 0 ||
        charutils::delLearnedAbility(&character, firstID) != 1 ||
        charutils::hasLearnedAbility(&character, firstID) != 0 ||
        charutils::delLearnedAbility(&character, firstID) != 0)
    {
        return false;
    }

    if (charutils::addLearnedAbility(&character, lastID) != 1 ||
        charutils::hasLearnedAbility(&character, lastID) == 0 ||
        charutils::delLearnedAbility(&character, lastID) != 1 ||
        charutils::addLearnedAbility(&character, outOfRangeID) != 0 ||
        charutils::delLearnedAbility(&character, outOfRangeID) != 0)
    {
        return false;
    }

    return true;
}
} // namespace

auto runLearnedAbilityMutation6845SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "learned ability mutation 6845 self-test failed\n";
    }
    return ok;
}
