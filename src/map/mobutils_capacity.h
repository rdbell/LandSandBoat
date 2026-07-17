#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure mobutils / CMobEntity gil-eligibility dual-wire helpers.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2653: residual pure port (entities/mob_gil_policy.h mobgilhelpers)
//   - 2960: CanDropGil (gilMin/gilMax/gilBonus eligibility gate)
//
// Production host: CMobEntity::CanDropGil / CanStealGil in mob_entity.cpp
// injects getMobMod(MOBMOD_GIL_MIN/MAX/BONUS) into
// mobgilhelpers::CanDropGil (same formula; residual 2653 surface).
//
// This capacity dual-wires the free-function form used by OmegaXI
// internal/mobutils (gil_policy.go) so hosts call CanDropGil instead of
// re-inlining:
//
//   gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0)
//
// CanStealGil dual-wires through the same free function (production
// CMobEntity::CanStealGil delegates to CanDropGil).
//
// Hosts inject the three gil mobmod scalars only (no CMobEntity*).
// Gil drop amount / steal amount writeback remains host-owned.
//
// Go dual-wire: mobutils.CanDropGil / mobutils.CanStealGil
// (internal/mobutils/gil_policy.go).

namespace mobutilshelpers
{

// CanDropGil mirrors CMobEntity::CanDropGil pure half (slice 2960):
//
//   gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0)
//
// A negative gil maximum is a suppression sentinel and takes precedence
// over minimum and bonus modifiers. Matches Go mobutils.CanDropGil and
// residual mobgilhelpers::CanDropGil (2653).
inline auto CanDropGil(const int16 gilMin, const int16 gilMax, const int16 gilBonus) -> bool
{
    return gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0);
}

// CanStealGil mirrors CMobEntity::CanStealGil, which delegates to
// CanDropGil (same free-function dual-wire; slice 2960).
inline auto CanStealGil(const int16 gilMin, const int16 gilMax, const int16 gilBonus) -> bool
{
    return CanDropGil(gilMin, gilMax, gilBonus);
}

} // namespace mobutilshelpers
