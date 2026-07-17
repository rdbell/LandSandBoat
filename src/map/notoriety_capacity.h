#pragma once

#include <cstddef>

// Pure CNotorietyContainer policy helpers.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2959: ShouldAddNotorietyMember residual dual-wire expand
//   - 3165: ShouldAddNotorietyMember dedicated dual-wire
//           (ownerPresent && entityPresent && differentAllegiance;
//            residual expand 2959 / pure 2818)
//   - 2971: ShouldRemoveNotorietyMember residual dual-wire expand
//   - 3192: ShouldRemoveNotorietyMember dedicated dual-wire
//           (ownerPresent && entityPresent;
//            residual expand 2971 / pure 2819)
//   - 3020: ShouldScanNotorietyForPrune (hasEnmity outer gate two-bool AND)
//   - 3029: ShouldPruneMobFromNotoriety (hasEnmity per-entry prune four-bool)
//   - 3034: HasEnmityAfterPrune (hasEnmity final empty report NOT empty)
//   - residual 2807: hasEnmity stale-mob prune gates
//     (ShouldScanNotorietyForPrune dual-wired as 3020;
//      ShouldPruneMobFromNotoriety dual-wired as 3029)
//   - residual 2818: add admission (prior pure port of ShouldAddNotorietyMember)
//   - residual 2819: remove admission (prior pure port of ShouldRemoveNotorietyMember)
//   - residual 2832: hasEnmity / size pure reporting
//     (HasEnmityAfterPrune dual-wired as 3034; NotorietySize remains residual)
//
// Production host: CNotorietyContainer in notoriety_container.cpp.
// Helpers take host-injected scalars/bools only (no entity/enmity pointers).
// Go dual-wire: notoriety.ShouldAddNotorietyMember
// (internal/notoriety/add_member.go). Prior pure port: slice 2818.
// Residual dual-wire suite: 2959 / test_notoriety_add_member_2959.
// Dedicated dual-wire suite: 3165 / test_notoriety_add_member_3165.
// Go dual-wire: notoriety.ShouldRemoveNotorietyMember
// (internal/notoriety/remove_member.go). Prior pure port: slice 2819.
// Residual dual-wire suite: 2971 / test_notoriety_remove_member_2971.
// Dedicated dual-wire suite: 3192 / test_notoriety_remove_member_3192.
// Go dual-wire: notoriety.ShouldScanNotorietyForPrune
// (internal/notoriety/scan_prune.go). Prior pure port: slice 2807.
// Go dual-wire: notoriety.ShouldPruneMobFromNotoriety
// (internal/notoriety/prune_mob.go). Prior pure port: slice 2807.
// Go dual-wire: notoriety.HasEnmityAfterPrune
// (internal/notoriety/has_enmity_after_prune.go). Prior pure port: slice 2832.

namespace notorietyhelpers
{

// ShouldScanNotorietyForPrune mirrors the hasEnmity outer gate (~81):
//   m_POwner && !m_Lookup.empty()
//
// Formula (slice 3020 dual-wire):
//   ownerPresent && lookupNonEmpty
//
// Host-injected scalars (no entity / set pointers):
//   ownerPresent    — m_POwner != nullptr
//   lookupNonEmpty  — !m_Lookup.empty()
// true  → host may walk m_Lookup and prune stale mobs
// false → host skips the prune walk and reports !lookup.empty()
//
// Dual-wire of Go notoriety.ShouldScanNotorietyForPrune
// (internal/notoriety/scan_prune.go). Prior pure port: slice 2807.
// Call site: CNotorietyContainer::hasEnmity (notoriety_container.cpp).
inline auto ShouldScanNotorietyForPrune(const bool ownerPresent, const bool lookupNonEmpty) -> bool
{
    return ownerPresent && lookupNonEmpty;
}

// ShouldPruneMobFromNotoriety mirrors the per-entry prune condition inside the
// hasEnmity walk after dynamic_cast<CMobEntity*> (~98):
//
//   if !isMob → never prune in this loop (non-mobs retained)
//   if isMob  → (isAlive && notOnEnmityList) || isDead
//
// Formula (slice 3029 dual-wire):
//   !isMob → false
//   else   → (isAlive && notOnEnmityList) || isDead
//
// Host-injected scalars (no entity / enmity pointers):
//   isMob            — dynamic_cast<CMobEntity*>(entry) != nullptr
//   isAlive          — mob->isAlive() when isMob; else false
//   isDead           — mob->isDead() when isMob; else false
//   notOnEnmityList  — enmity list missing owner id (uint16) when isMob;
//                      else false
// isAlive and isDead are injected separately as production does; both may be
// false for a transitional host (neither alive nor dead → retain).
// true  → host may remove the entry from m_Lookup (stale mob)
// false → host retains the entry (non-mob, live with owner on list, or
//         transitional)
//
// Dual-wire of Go notoriety.ShouldPruneMobFromNotoriety
// (internal/notoriety/prune_mob.go). Prior pure port: slice 2807.
// Call site: CNotorietyContainer::hasEnmity (notoriety_container.cpp).
// Sibling outer gate: ShouldScanNotorietyForPrune (slice 3020).
inline auto ShouldPruneMobFromNotoriety(
    const bool isMob,
    const bool isAlive,
    const bool isDead,
    const bool notOnEnmityList) -> bool
{
    if (!isMob)
    {
        return false;
    }
    return (isAlive && notOnEnmityList) || isDead;
}

// ShouldAddNotorietyMember mirrors CNotorietyContainer::add admission (~48):
//   m_POwner && entity && entity->allegiance != m_POwner->allegiance
//
// Formula (slice 3165 dedicated dual-wire; residual expand 2959 / pure 2818 —
// formula unchanged):
//   ownerPresent && entityPresent && differentAllegiance
//
// Host-injected scalars (no entity pointers):
//   ownerPresent        — m_POwner != nullptr
//   entityPresent       — entity != nullptr
//   differentAllegiance — host-safe: false when either pointer is null,
//                         otherwise entity->allegiance != m_POwner->allegiance
// true  → host may insert entity into m_Lookup (std::set pointer identity;
//         duplicates are no-ops)
// false → add is a no-op (nil owner/entity or same allegiance)
//
// Dual-wire of Go notoriety.ShouldAddNotorietyMember
// (internal/notoriety/add_member.go). Prior pure port: slice 2818.
// Residual dual-wire suite: 2959 / test_notoriety_add_member_2959.
// Dedicated dual-wire suite is test_notoriety_add_member_3165. Formula is
// unchanged; this slice only expands dual-wire docs + index + dedicated suite.
// Call site: CNotorietyContainer::add (notoriety_container.cpp).
inline auto ShouldAddNotorietyMember(
    const bool ownerPresent,
    const bool entityPresent,
    const bool differentAllegiance) -> bool
{
    return ownerPresent && entityPresent && differentAllegiance;
}

// ShouldRemoveNotorietyMember mirrors CNotorietyContainer::remove admission (~60):
//   m_POwner && entity
//
// Formula (slice 3192 dedicated dual-wire; residual expand 2971 / pure 2819 —
// formula unchanged):
//   ownerPresent && entityPresent
//
// Host-injected scalars (no entity pointers):
//   ownerPresent  — m_POwner != nullptr
//   entityPresent — entity != nullptr
// true  → host may find/erase the entity from m_Lookup (std::set pointer
//         identity; absent keys are no-ops)
// false → remove is a no-op (nil owner or nil entity)
//
// Dual-wire of Go notoriety.ShouldRemoveNotorietyMember
// (internal/notoriety/remove_member.go). Prior pure port: slice 2819.
// Residual dual-wire suite: 2971 / test_notoriety_remove_member_2971.
// Dedicated dual-wire suite is test_notoriety_remove_member_3192. Formula is
// unchanged; this slice only expands dual-wire docs + index + dedicated suite.
// Call site: CNotorietyContainer::remove (notoriety_container.cpp).
// Sibling left alone: ShouldAddNotorietyMember (3165).
inline auto ShouldRemoveNotorietyMember(const bool ownerPresent, const bool entityPresent) -> bool
{
    return ownerPresent && entityPresent;
}

// HasEnmityAfterPrune mirrors CNotorietyContainer::hasEnmity final return (~111)
// after the optional stale-mob prune walk:
//   !m_Lookup.empty()
//
// Formula (slice 3034 dual-wire):
//   !lookupEmpty
//
// Host-injected scalar (no set pointers):
//   lookupEmpty  — m_Lookup.empty() after the optional prune walk (or when
//                  the prune gate skipped)
// true  → reverse list still has at least one entry (has enmity)
// false → lookup empty after prune (or was already empty)
//
// Dual-wire of Go notoriety.HasEnmityAfterPrune
// (internal/notoriety/has_enmity_after_prune.go). Prior pure port: slice 2832.
// Call site: CNotorietyContainer::hasEnmity (notoriety_container.cpp).
// Sibling outer scan / per-entry prune: ShouldScanNotorietyForPrune (3020) /
// ShouldPruneMobFromNotoriety (3029). Residual size reporting: NotorietySize
// (2832).
inline auto HasEnmityAfterPrune(const bool lookupEmpty) -> bool
{
    return !lookupEmpty;
}

// NotorietySize mirrors CNotorietyContainer::size (~113):
//   m_Lookup.size()
//
// Identity pure: host injects the count; helper returns it unchanged so size
// reporting dual-wires through the same pure surface as hasEnmity's empty
// report.
inline auto NotorietySize(const std::size_t count) -> std::size_t
{
    return count;
}

} // namespace notorietyhelpers
