#pragma once

namespace mobcontrollermobskilltarget
{
enum class Target
{
    None,
    Enemy,
    Self,
};

// Select mirrors CMobController::MobSkill's valid-target priority.
constexpr auto Select(const bool enemyValid, const bool selfValid) -> Target
{
    if (enemyValid)
    {
        return Target::Enemy;
    }
    return selfValid ? Target::Self : Target::None;
}
} // namespace mobcontrollermobskilltarget
