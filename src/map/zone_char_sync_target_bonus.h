#pragma once

namespace zonecharsynctargetbonus
{

// ShouldAward mirrors SpawnPCs' target-bonus admission after a mob state has
// supplied its target.
constexpr auto ShouldAward(const bool hasTarget, const bool targetIsCharacter, const bool targetIsOrigin) -> bool
{
    return hasTarget && targetIsCharacter && !targetIsOrigin;
}

} // namespace zonecharsynctargetbonus
