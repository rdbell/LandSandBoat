#pragma once

#include <cstdint>

// Pure stealth/invisibility cleanup policy from CCharEntity::OnAbility after
// paralysis. Host applies RemoveInvisible / detectable-flag / Illusion effects.

namespace charabilitystealthhelpers
{

// Ability ID pins.
constexpr std::uint16_t AbilityGauge      = 53;
constexpr std::uint16_t AbilityTame       = 54;
constexpr std::uint16_t AbilityFight      = 69;
constexpr std::uint16_t AbilityTrickAttack = 76;
constexpr std::uint16_t AbilityAssault    = 88;
constexpr std::uint16_t AbilityDeploy     = 138;

// TARGET_ENEMY pin from battle_entity.h.
constexpr std::uint16_t TargetEnemy = 0x0004;

struct Plan
{
    bool removeInvisible{};  // charutils::RemoveInvisible
    bool removeDetectable{}; // DelStatusEffectsByFlag(Detectable)
    bool removeIllusion{};
};

// PlanCleanup mirrors the post-paralysis stealth branch of OnAbility.
constexpr auto PlanCleanup(const std::uint16_t abilityID, const std::uint16_t validTarget) -> Plan
{
    if (abilityID == AbilityTame || abilityID == AbilityFight || abilityID == AbilityDeploy || abilityID == AbilityGauge)
    {
        return {};
    }

    Plan plan{};
    if ((validTarget & TargetEnemy) != 0)
    {
        if (abilityID == AbilityAssault)
        {
            plan.removeInvisible = true;
        }
        else
        {
            plan.removeDetectable = true;
        }
        plan.removeIllusion = true;
        return plan;
    }

    if (abilityID != AbilityTrickAttack)
    {
        plan.removeInvisible = true;
        plan.removeIllusion  = true;
    }
    return plan;
}

} // namespace charabilitystealthhelpers
