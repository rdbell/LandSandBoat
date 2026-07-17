#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure MOBTYPE pin constants + free helpers shared by dual-wire slices:
//   - 2919: CanDeaggro (type-bit gate)
//   - 2934: CanBeNeutral (type-bit gate)
//
// Production call sites today:
//   - CMobEntity::CanDeaggro in mob_entity.cpp routes through
//     mobbehaviorhelpers::CanDeaggro(notorious, battlefield) after
//     host-extracting m_Type & MOBTYPE_NOTORIOUS / MOBTYPE_BATTLEFIELD.
//   - CMobEntity::CanBeNeutral in mob_entity.cpp routes through
//     mobbehaviorhelpers::CanBeNeutral(notorious) after host-extracting
//     m_Type & MOBTYPE_NOTORIOUS.
//
// This capacity dual-wires the Type-bit form used by OmegaXI
// internal/mobtype (slice 2042 residual / 2919 / 2934 dual-wire):
//
//   CanDeaggro(t)    = !Has(t, Notorious) && !Has(t, Battlefield)
//   CanBeNeutral(t)  = !Has(t, Notorious)
//
// Hosts inject the raw m_Type / MOBTYPE byte. Side effects (deaggro
// timers, controller tick, neutral timer) remain host-owned.
//
// Reference: src/map/entities/mob_entity.h enum MOBTYPE
//   MOBTYPE_NORMAL      = 0x00
//   MOBTYPE_0X01        = 0x01
//   MOBTYPE_NOTORIOUS   = 0x02
//   MOBTYPE_FISHED      = 0x04
//   MOBTYPE_CALLED      = 0x08
//   MOBTYPE_BATTLEFIELD = 0x10
//   MOBTYPE_EVENT       = 0x20
//
// Reference: src/map/entities/mob_entity.cpp CMobEntity::CanDeaggro
//   return mobbehaviorhelpers::CanDeaggro(m_Type & MOBTYPE_NOTORIOUS,
//                                         m_Type & MOBTYPE_BATTLEFIELD);
//
// Reference: src/map/entities/mob_entity.cpp CMobEntity::CanBeNeutral
//   return mobbehaviorhelpers::CanBeNeutral(m_Type & MOBTYPE_NOTORIOUS);

namespace mobtypehelpers
{

// ---------------------------------------------------------------------------
// MOBTYPE pin constants (mob_entity.h enum MOBTYPE)
// ---------------------------------------------------------------------------

inline constexpr uint8 Normal      = 0x00; // MOBTYPE_NORMAL
inline constexpr uint8 Available01 = 0x01; // MOBTYPE_0X01 (unused reserve)
inline constexpr uint8 Notorious   = 0x02; // MOBTYPE_NOTORIOUS
inline constexpr uint8 Fished      = 0x04; // MOBTYPE_FISHED
inline constexpr uint8 Called      = 0x08; // MOBTYPE_CALLED
inline constexpr uint8 Battlefield = 0x10; // MOBTYPE_BATTLEFIELD
inline constexpr uint8 Event       = 0x20; // MOBTYPE_EVENT

// ---------------------------------------------------------------------------
// Shared bit helper
// ---------------------------------------------------------------------------

// Has reports whether type byte t includes every bit of flag
// (mirrors OmegaXI mobtype.Has: t & flag == flag).
inline auto Has(const uint8 t, const uint8 flag) -> bool
{
    return (t & flag) == flag;
}

// ---------------------------------------------------------------------------
// Slice 2919 — CanDeaggro type-bit gate
// ---------------------------------------------------------------------------

// CanDeaggro mirrors CMobEntity::CanDeaggro's type policy:
//
//   !Has(t, Notorious) && !Has(t, Battlefield)
//
// Notorious and battlefield mobs stay engaged; all other type combinations
// can deaggro. Host still owns controller deaggro timers / tick.
// Future host inject may call this free function with raw m_Type instead of
// splitting notorious/battlefield bools at the call site.
inline auto CanDeaggro(const uint8 t) -> bool
{
    return !Has(t, Notorious) && !Has(t, Battlefield);
}

// ---------------------------------------------------------------------------
// Slice 2934 — CanBeNeutral type-bit gate
// ---------------------------------------------------------------------------

// CanBeNeutral mirrors CMobEntity::CanBeNeutral's type policy:
//
//   !Has(t, Notorious)
//
// Only notorious mobs are excluded from the neutral/killing-pause behavior.
// Battlefield alone may still be neutral. Host still owns controller neutral
// timer / tick. Future host inject may call this free function with raw
// m_Type instead of splitting the notorious bool at the call site.
inline auto CanBeNeutral(const uint8 t) -> bool
{
    return !Has(t, Notorious);
}

} // namespace mobtypehelpers
