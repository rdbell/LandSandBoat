#pragma once

namespace battlespawnhelpers
{

template <typename ResetAnimation, typename ShowName, typename SpawnBase, typename ClearOwner, typename ClearBattleID>
inline void Apply(
    ResetAnimation&& resetAnimation,
    ShowName&& showName,
    SpawnBase&& spawnBase,
    ClearOwner&& clearOwner,
    ClearBattleID&& clearBattleID)
{
    resetAnimation();
    showName();
    spawnBase();
    clearOwner();
    clearBattleID();
}

} // namespace battlespawnhelpers
