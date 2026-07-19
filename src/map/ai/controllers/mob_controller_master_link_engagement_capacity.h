#pragma once

namespace mobcontrollermasterlinkengagement
{
// CanEngage reports whether a mob's master joins its current fight.
template <typename IsOuterRoaming, typename IsInnerRoaming, typename CanLink>
constexpr auto CanEngage(
    const bool hasMaster,
    IsOuterRoaming&& isOuterRoaming,
    IsInnerRoaming&& isInnerRoaming,
    CanLink&& canLink) -> bool
{
    return hasMaster && isOuterRoaming() && isInnerRoaming() && canLink();
}
} // namespace mobcontrollermasterlinkengagement
