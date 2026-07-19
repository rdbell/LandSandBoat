#pragma once

namespace trustcontrollercombatwarpadmission
{
constexpr float WarpDistance = 30.0f;

constexpr auto ShouldWarp(const bool masterEngaged, const float distanceToTarget) -> bool
{
    return !masterEngaged && distanceToTarget > WarpDistance;
}
} // namespace trustcontrollercombatwarpadmission
