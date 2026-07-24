#pragma once

namespace inactivezerodurationexit
{
template <typename IsDead, typename HasPreventAction, typename HasCharm, typename HasPreventActionIgnoringCharm>
constexpr auto shouldExit(
    IsDead&& isDead,
    HasPreventAction&& hasPreventAction,
    HasCharm&& hasCharm,
    HasPreventActionIgnoringCharm&& hasPreventActionIgnoringCharm) -> bool
{
    if (isDead())
    {
        return true;
    }
    if (!hasPreventAction())
    {
        return true;
    }
    return hasCharm() && !hasPreventActionIgnoringCharm();
}

constexpr auto shouldExit(
    const bool isDead,
    const bool hasPreventAction,
    const bool hasCharm,
    const bool hasPreventActionIgnoringCharm) -> bool
{
    return shouldExit(
        [&]() { return isDead; },
        [&]() { return hasPreventAction; },
        [&]() { return hasCharm; },
        [&]() { return hasPreventActionIgnoringCharm; });
}
} // namespace inactivezerodurationexit
