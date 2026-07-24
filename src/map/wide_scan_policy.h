#pragma once

namespace widescanhelpers
{

// ShouldIncludeWideScanEntity mirrors CZoneEntities::WideScan's list filter.
inline auto ShouldIncludeWideScanEntity(const bool wideScannable, const bool inRange, const bool sameFloor) -> bool
{
    return wideScannable && inRange && sameFloor;
}

} // namespace widescanhelpers
