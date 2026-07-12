#pragma once

#include <cstdint>

// Pure pet-ability target selection from CCharEntity::OnAbility pet branch.
// Host retains pet skill lookup, packet assembly, and PetSkill dispatch.

namespace charabilitypethelpers
{

// TARGET_ENEMY pin.
constexpr std::uint16_t TargetEnemy = 0x0004;

// ActionPacketTargetID: jug pets target the master on the JA finish packet;
// other pets target the ability target.
constexpr auto ActionPacketTargetID(const bool isJugPet,
                                    const std::uint32_t masterID,
                                    const std::uint32_t abilityTargetID) -> std::uint32_t
{
    return isJugPet ? masterID : abilityTargetID;
}

// PetSkillTargetID selects the entity the pet should skill against.
// Non-jug pets use the ability target. Jug pets with enemy-valid skills use
// their battle target; otherwise they target themselves.
constexpr auto PetSkillTargetID(const bool isJugPet,
                                const bool skillTargetsEnemy,
                                const std::uint16_t abilityTargetTargID,
                                const std::uint16_t petBattleTargetID,
                                const std::uint16_t petTargID) -> std::uint16_t
{
    if (!isJugPet)
    {
        return abilityTargetTargID;
    }
    if (skillTargetsEnemy)
    {
        return petBattleTargetID;
    }
    return petTargID;
}

} // namespace charabilitypethelpers
