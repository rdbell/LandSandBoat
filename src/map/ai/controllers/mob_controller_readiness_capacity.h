#pragma once

#include <chrono>

namespace mobcontrollerreadiness
{
constexpr auto SpellReady(
    const bool chainspell,
    const bool manafont,
    const bool isWorm,
    const float distance,
    const float meleeRange,
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::steady_clock::time_point nextMagicTime,
    const std::chrono::steady_clock::duration standbackCooldown) -> bool
{
    if (chainspell || manafont)
    {
        return true;
    }
    if (isWorm && distance <= meleeRange)
    {
        return false;
    }
    return distance > 5 && !isWorm ? tick >= nextMagicTime - standbackCooldown : tick >= nextMagicTime;
}

constexpr auto SpecialSkillReady(
    const bool hasSpecialSkill,
    const bool chainspell,
    const float distance,
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::steady_clock::time_point lastSpecialTime,
    const std::chrono::steady_clock::duration cooldown,
    const std::chrono::steady_clock::duration standbackCooldown) -> bool
{
    if (!hasSpecialSkill || chainspell)
    {
        return false;
    }
    const auto adjustedCooldown = cooldown - (distance > 5 ? standbackCooldown : std::chrono::steady_clock::duration(0));
    return tick >= lastSpecialTime + adjustedCooldown;
}
} // namespace mobcontrollerreadiness
