#pragma once

#include "common/cbasetypes.h"

// Pure UpdateEnmity non-mob holder early-return gate for dual-wire slices:
//   - 1357: residual pure port (enmitymath in enmity_container.h)
//   - 2956: ShouldRejectNonMobHolder residual dual-wire suite
//   - 3182: ShouldRejectNonMobHolder dedicated dual-wire (reject_non_mob.go)
//
// Dual-wire index:
//   - 2956: ShouldRejectNonMobHolder residual dual-wire suite
//   - 3182: ShouldRejectNonMobHolder = !holderIsMob (identity-not)
//
// Production host today: CEnmityContainer::UpdateEnmity in enmity_container.cpp
// routes through enmitymath::ShouldRejectNonMobHolder(
//     m_EnmityHolder->objtype == ENTITYTYPE::TYPE_MOB)
// (same formula; residual pure port slice 1357 in enmity_container.h).
//
// This capacity dual-wires the free-function form used by OmegaXI
// internal/enmity (reject_non_mob.go) so hosts call
// ShouldRejectNonMobHolder instead of re-inlining !holderIsMob, and so
// production can inject:
//
//   ShouldRejectNonMobHolder(m_EnmityHolder->objtype == TYPE_MOB)
//
// Hosts inject the host-evaluated (objtype == TYPE_MOB) boolean. Side effects
// (early return from UpdateEnmity for pets/trusts) remain host-owned.
//
// Go dual-wire: enmity.ShouldRejectNonMobHolder (internal/enmity/reject_non_mob.go).
// Residual dual-wire suite: 2956.
// Dedicated dual-wire suite: 3182.
// Prior pure port: OmegaXI slice 1357 (internal/enmity).
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
// Slice 3182 — ShouldRejectNonMobHolder non-mob holder early-return gate
// (dedicated dual-wire; residual expand 2956 / pure 1357 — formula unchanged)
// ---------------------------------------------------------------------------

// ShouldRejectNonMobHolder mirrors holder objtype != TYPE_MOB early return.
//
// Formula (slice 3182 dedicated dual-wire; residual expand 2956 / pure 1357 —
// formula unchanged):
//   ShouldRejectNonMobHolder(holderIsMob) = !holderIsMob
//
// holderIsMob is host-evaluated (m_EnmityHolder->objtype == TYPE_MOB).
// true  → accept holder; continue UpdateEnmity (holder is a mob)
// false → early return; pets and trusts do not hold enmity
//
// Dual-wire of Go enmity.ShouldRejectNonMobHolder.
// Production call site: CEnmityContainer::UpdateEnmity (via enmitymath).
// Residual dual-wire suite: 2956. Dedicated dual-wire suite: 3182.
inline auto ShouldRejectNonMobHolder(const bool holderIsMob) -> bool
{
    return !holderIsMob;
}

} // namespace enmityrejecthelpers
