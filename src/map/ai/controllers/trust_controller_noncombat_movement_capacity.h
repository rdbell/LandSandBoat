#pragma once

#include <cstdint>

namespace trustcontrollernoncombatmovement
{

enum class Action : uint8_t
{
    Hold,
    Path,
    Step,
    Warp,
};

inline auto Resolve(float currentDistance, float desiredDistance) -> Action
{
    if (currentDistance > 30.0f)
    {
        return Action::Warp;
    }
    if (currentDistance > desiredDistance && currentDistance < desiredDistance * 3.0f)
    {
        return Action::Path;
    }
    if (currentDistance > desiredDistance)
    {
        return Action::Step;
    }
    return Action::Hold;
}

} // namespace trustcontrollernoncombatmovement
