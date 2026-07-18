#pragma once

namespace petcontrollertick
{

inline auto ShouldDespawnForCharm(bool isPlayerPet, bool isCharmed, bool charmExpired) -> bool
{
    return isPlayerPet && isCharmed && charmExpired;
}

inline auto ShouldDespawnForJug(bool isPlayerPet, bool isPetEntity, bool isAlive, bool isJugPet, bool jugExpired) -> bool
{
    return isPlayerPet && isPetEntity && isAlive && isJugPet && jugExpired;
}

} // namespace petcontrollertick
