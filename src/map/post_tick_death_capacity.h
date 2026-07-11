#pragma once

namespace posttickdeathhelpers
{

inline auto ShouldDie(const int hp, const bool spawned, const bool inDeathState, const bool inDespawnState) -> bool
{
    return hp <= 0 && spawned && !inDeathState && !inDespawnState;
}

} // namespace posttickdeathhelpers
