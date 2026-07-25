#include "test_weaponskill_mutation_6849.h"

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
    constexpr auto lastID       = std::uint16_t{ sizeof(character.m_WeaponSkills) * 8 - 1 };
    constexpr auto outOfRangeID = std::uint16_t{ sizeof(character.m_WeaponSkills) * 8 };

    if (charutils::hasWeaponSkill(&character, firstID) != 0 ||
        charutils::addWeaponSkill(&character, firstID) != 1 ||
        charutils::hasWeaponSkill(&character, firstID) == 0 ||
        charutils::addWeaponSkill(&character, firstID) != 0 ||
        charutils::delWeaponSkill(&character, firstID) != 1 ||
        charutils::hasWeaponSkill(&character, firstID) != 0 ||
        charutils::delWeaponSkill(&character, firstID) != 0)
    {
        return false;
    }

    return charutils::addWeaponSkill(&character, lastID) == 1 &&
           charutils::hasWeaponSkill(&character, lastID) != 0 &&
           charutils::delWeaponSkill(&character, lastID) == 1 &&
           charutils::addWeaponSkill(&character, outOfRangeID) == 0 &&
           charutils::delWeaponSkill(&character, outOfRangeID) == 0;
}
} // namespace

auto runWeaponSkillMutation6849SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "weaponskill mutation 6849 self-test failed\n";
    }
    return ok;
}
