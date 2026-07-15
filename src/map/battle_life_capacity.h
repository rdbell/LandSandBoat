#pragma once

#include <cstdint>

// Pure CBattleEntity::isDead / isAlive with fully injected inputs.
// Parity: internal/battlelife (slice 1659).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::isDead  (~156–159)
//   CBattleEntity::isAlive (~161–164)
//
// Host retains health/status storage and PAI state machine; helpers take
// injected values only. PAI->IsCurrentState<CDeathState/CDespawnState>() is
// expressed as boolean injects so this header stays free of AI state coupling.

namespace battlelifehelpers
{

// STATUS_TYPE::DISAPPEAR (2). Duplicated pin; matches aistate StatusDisappear.
constexpr std::uint8_t StatusDisappear = 2;

// IsDead mirrors CBattleEntity::isDead:
//   health.hp <= 0
//   || status == STATUS_TYPE::DISAPPEAR
//   || PAI->IsCurrentState<CDeathState>()
//   || PAI->IsCurrentState<CDespawnState>()
inline auto IsDead(const std::int32_t hp,
                   const std::uint8_t  status,
                   const bool          inDeathState,
                   const bool          inDespawnState) -> bool
{
    return hp <= 0 || status == StatusDisappear || inDeathState || inDespawnState;
}

// IsAlive mirrors CBattleEntity::isAlive: return !isDead().
inline auto IsAlive(const std::int32_t hp,
                    const std::uint8_t  status,
                    const bool          inDeathState,
                    const bool          inDespawnState) -> bool
{
    return !IsDead(hp, status, inDeathState, inDespawnState);
}

} // namespace battlelifehelpers
