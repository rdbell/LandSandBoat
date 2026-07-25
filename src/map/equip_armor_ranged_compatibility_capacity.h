#pragma once

#include <cstdint>

// Pure ranged/ammo compatibility rule from charutils::EquipArmor.

namespace equiparmorrangedhelpers
{

constexpr std::uint8_t ArcherySkill = 25;

struct Facts
{
    bool          incomingIsWeapon{};
    bool          otherIsWeapon{};
    std::uint8_t  incomingSkill{};
    std::uint8_t  incomingSubSkill{};
    std::uint8_t  otherSkill{};
    std::uint8_t  otherSubSkill{};
};

// ShouldUnequipOther mirrors the shared ranged/ammo compatibility check.
constexpr auto ShouldUnequipOther(const Facts& facts) -> bool
{
    if (!facts.incomingIsWeapon || !facts.otherIsWeapon)
    {
        return false;
    }
    if (facts.incomingSkill != facts.otherSkill)
    {
        return true;
    }
    return facts.otherSkill != ArcherySkill && facts.incomingSubSkill != facts.otherSubSkill;
}

} // namespace equiparmorrangedhelpers
