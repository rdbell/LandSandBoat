#pragma once

namespace mobcontrollerrunaway
{
struct Action
{
    bool handled;
    bool startPath;
    bool followPath;
    bool notifyArrival;
    bool clearTarget;
};

// Resolve mirrors DoCombatTick's run-away follow handling.
constexpr auto Resolve(const bool hasRunAwayTarget, const bool beyondDistance, const bool followingPath) -> Action
{
    if (!hasRunAwayTarget)
    {
        return {};
    }
    if (beyondDistance)
    {
        return { true, !followingPath, true, false, false };
    }
    return { true, false, false, true, true };
}
} // namespace mobcontrollerrunaway
