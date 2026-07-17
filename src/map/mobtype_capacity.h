#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure MOBTYPE pin constants + free helpers shared by dual-wire slices:
//   - 2919: CanDeaggro residual dual-wire suite (type-bit gate)
//   - 2934: CanBeNeutral residual dual-wire suite (type-bit gate)
//   - 3063: CanDeaggro dedicated dual-wire (can_deaggro.go)
//   - 3076: CanBeNeutral dedicated dual-wire (can_be_neutral.go)
//   - 3223: CanBeNeutral dedicated dual-wire expand residual 2934
//   - 3276: CanBeNeutral dedicated dual-wire expand residual 2934 (prior ~3223)
//   - 3306: CanBeNeutral dedicated dual-wire expand residual 2934 (prior ~3276)
//
// Dual-wire index:
//   - 2919: CanDeaggro residual dual-wire (Type-bit form; dense suite)
//   - 2934: CanBeNeutral residual dual-wire (Type-bit form; dense suite)
//   - 3063: CanDeaggro (!Has(t, Notorious) && !Has(t, Battlefield);
//           dedicated dual-wire on can_deaggro.go)
//   - 3076: CanBeNeutral (!Has(t, Notorious);
//           dedicated dual-wire on can_be_neutral.go)
//   - 3223: CanBeNeutral (!Has(t, Notorious);
//           dedicated dual-wire expand residual 2934 on can_be_neutral.go)
//   - 3276: CanBeNeutral (!Has(t, Notorious);
//           dedicated dual-wire expand residual 2934 / prior ~3223 on
//           can_be_neutral.go)
//   - 3306: CanBeNeutral (!Has(t, Notorious);
//           dedicated dual-wire expand residual 2934 / prior ~3276 on
//           can_be_neutral.go)
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
// internal/mobtype (slice 2042 residual / 2919 / 2934 / 3063 / 3076 / 3223
// / 3276 / 3306 dual-wire):
//
//   CanDeaggro(t)    = !Has(t, Notorious) && !Has(t, Battlefield)
//   CanBeNeutral(t)  = !Has(t, Notorious)
//
// Hosts inject the raw m_Type / MOBTYPE byte. Side effects (deaggro
// timers, controller tick, neutral timer) remain host-owned.
// Go dual-wire: mobtype.CanDeaggro (internal/mobtype/can_deaggro.go),
// mobtype.CanBeNeutral (internal/mobtype/can_be_neutral.go).
// Future host injects mobtypehelpers::CanDeaggro(m_Type) then deaggro path,
// or mobtypehelpers::CanBeNeutral(m_Type) then neutral path.
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
// Slice 2919 / 3063 — CanDeaggro type-bit gate
// ---------------------------------------------------------------------------

// CanDeaggro mirrors CMobEntity::CanDeaggro's type policy:
//
//   !Has(t, Notorious) && !Has(t, Battlefield)
//
// Formula (slice 3063 dual-wire; residual expand 2919):
//   CanDeaggro(t) = !Has(t, Notorious) && !Has(t, Battlefield)
//
// Notorious and battlefield mobs stay engaged; all other type combinations
// can deaggro. Host still owns controller deaggro timers / tick.
// Dual-wire of Go mobtype.CanDeaggro.
// Call site: future CMobEntity::CanDeaggro inject with raw m_Type.
// Prior pure port: slices 2042 / 2655. Residual dual-wire suite: 2919 /
// test_mobtype_can_deaggro_2919. Dedicated dual-wire suite is
// test_mobtype_can_deaggro_3063. Future host inject may call this free
// function with raw m_Type instead of splitting notorious/battlefield
// bools at the call site.
inline auto CanDeaggro(const uint8 t) -> bool
{
    return !Has(t, Notorious) && !Has(t, Battlefield);
}

// ---------------------------------------------------------------------------
// Slice 2934 / 3076 / 3223 / 3276 / 3306 — CanBeNeutral type-bit gate
// ---------------------------------------------------------------------------

// CanBeNeutral mirrors CMobEntity::CanBeNeutral's type policy:
//
//   !Has(t, Notorious)
//
// Formula (slice 3306 dedicated dual-wire expand residual 2934; prior
// dedicated expand residual 3276 / 3223 / dedicated 3076 / pure 2042 / 2655 —
// formula unchanged):
//   CanBeNeutral(t) = !Has(t, Notorious)
//
// Only notorious mobs are excluded from the neutral/killing-pause behavior.
// Battlefield alone may still be neutral. Host still owns controller neutral
// timer / tick. Dual-wire of Go mobtype.CanBeNeutral.
// Call site: future CMobEntity::CanBeNeutral inject with raw m_Type.
// Prior pure port: slices 2042 / 2655. Residual dual-wire suite: 2934 /
// test_mobtype_can_be_neutral_2934. Prior dedicated dual-wire suite: 3076 /
// test_mobtype_can_be_neutral_3076. Prior dedicated expand residual suite:
// 3223 / test_mobtype_can_be_neutral_3223. Prior dedicated expand residual
// suite: 3276 / test_mobtype_can_be_neutral_3276. Dedicated expand residual
// suite is test_mobtype_can_be_neutral_3306. Future host inject may call
// this free function with raw m_Type instead of splitting the notorious
// bool at the call site.
inline auto CanBeNeutral(const uint8 t) -> bool
{
    return !Has(t, Notorious);
}

} // namespace mobtypehelpers
