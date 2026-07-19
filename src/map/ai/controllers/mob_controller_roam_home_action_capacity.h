#pragma once

namespace mobcontrollerroamhomeaction
{
enum class Action
{
    None,
    PathHome,
    Despawn,
};

// Resolve chooses the far-from-home roam action, preserving path-home priority.
constexpr auto Resolve(const bool shouldCheckHome, const bool canRoamHome, const bool canDespawn) -> Action
{
    if (!shouldCheckHome)
    {
        return Action::None;
    }
    if (canRoamHome)
    {
        return Action::PathHome;
    }
    if (canDespawn)
    {
        return Action::Despawn;
    }
    return Action::None;
}
} // namespace mobcontrollerroamhomeaction
