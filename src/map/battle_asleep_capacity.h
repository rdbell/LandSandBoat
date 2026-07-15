#pragma once

// Pure CBattleEntity::isAsleep with fully injected inputs.
// Parity: internal/battleasleep (slice 1674).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::isAsleep (~250–253)
//
// Host retains PAI state machine; helpers take injected values only.
// PAI->IsCurrentState<CInactiveState>() is expressed as a boolean inject so
// this header stays free of AI state coupling.
//
// Distinct from status_effect_capacity IsAsleep / IsAsleepEffectID (sleep-family
// effect membership). Entity isAsleep answers whether AI is in CInactiveState.

namespace battleasleephelpers
{

// IsAsleep mirrors CBattleEntity::isAsleep:
//   return PAI->IsCurrentState<CInactiveState>();
//
// inInactiveState is the host-projected IsCurrentState<CInactiveState>() flag.
inline auto IsAsleep(const bool inInactiveState) -> bool
{
    return inInactiveState;
}

} // namespace battleasleephelpers
