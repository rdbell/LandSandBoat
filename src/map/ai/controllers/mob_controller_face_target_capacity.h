#pragma once

#include <cstdint>

namespace mobcontrollerfacetarget
{
// ResolveTargetID selects an explicit target or the current battle target.
constexpr auto ResolveTargetID(const uint16_t targid, const uint16_t battleTargetID) -> uint16_t
{
    return targid != 0 ? targid : battleTargetID;
}

// ShouldLookAt reports whether a resolved target may be faced.
constexpr auto ShouldLookAt(const bool noTurn, const bool hasTarget) -> bool
{
    return !noTurn && hasTarget;
}
} // namespace mobcontrollerfacetarget
