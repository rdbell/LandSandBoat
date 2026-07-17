#pragma once

#include "common/cbasetypes.h"

// Pure UpdateEnmity non-mob holder early-return gate for dual-wire slices:
//   - 2956: ShouldRejectNonMobHolder (holderIsMob → !holderIsMob)
//
// Production host today: CEnmityContainer::UpdateEnmity in enmity_container.cpp
// routes through enmitymath::ShouldRejectNonMobHolder(
//     m_EnmityHolder->objtype == ENTITYTYPE::TYPE_MOB)
// (same formula; residual pure port slice 1357 in enmity_container.h).
//
// This capacity dual-wires the free-function form used by OmegaXI
// internal/enmity (slice 1357 residual / 2956 dual-wire):
//
//   ShouldRejectNonMobHolder(holderIsMob) = !holderIsMob
//
// Hosts inject the host-evaluated (objtype == TYPE_MOB) boolean. Side effects
// (early return from UpdateEnmity for pets/trusts) remain host-owned.
//
// Reference: src/map/enmity_container.cpp CEnmityContainer::UpdateEnmity
//   if (enmitymath::ShouldRejectNonMobHolder(
//           m_EnmityHolder->objtype == ENTITYTYPE::TYPE_MOB)) // pets and trusts
//   {
//       return;
//   }
//
// Reference: src/map/enmity_container.h enmitymath::ShouldRejectNonMobHolder
//   return !holderIsMob;

namespace enmityrejecthelpers
{

// ---------------------------------------------------------------------------
// Slice 2956 — ShouldRejectNonMobHolder non-mob holder early-return gate
// ---------------------------------------------------------------------------

// ShouldRejectNonMobHolder mirrors holder objtype != TYPE_MOB early return.
//
// Formula (slice 2956 dual-wire):
//   ShouldRejectNonMobHolder(holderIsMob) = !holderIsMob
//
// holderIsMob is host-evaluated (m_EnmityHolder->objtype == TYPE_MOB).
// true  → accept holder; continue UpdateEnmity (holder is a mob)
// false → early return; pets and trusts do not hold enmity
//
// Dual-wire of Go enmity.ShouldRejectNonMobHolder.
// Production call site: CEnmityContainer::UpdateEnmity (via enmitymath).
inline auto ShouldRejectNonMobHolder(const bool holderIsMob) -> bool
{
    return !holderIsMob;
}

} // namespace enmityrejecthelpers
