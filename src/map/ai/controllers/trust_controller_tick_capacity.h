#pragma once

namespace trustcontrollertick
{
enum class Route { None, Despawn, NonCombat, Combat, Roam };

constexpr auto Resolve(
    const bool hasMaster, const bool masterCharmed, const bool masterEngaged, const bool nonCombatFollow,
    const bool trustEngaged, const bool trustDead) -> Route
{
    if (!hasMaster) return Route::None;
    if (masterCharmed) return Route::Despawn;
    if (masterEngaged && nonCombatFollow) return Route::NonCombat;
    if (trustEngaged) return Route::Combat;
    if (!trustDead) return Route::Roam;
    return Route::None;
}
} // namespace trustcontrollertick
