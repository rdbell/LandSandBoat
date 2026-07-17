#pragma once

#include <cstddef>

// Pure CNotorietyContainer policy helpers.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2959: ShouldAddNotorietyMember (add admission three-bool AND)
//   - 2971: ShouldRemoveNotorietyMember (remove admission two-bool AND)
//   - 3020: ShouldScanNotorietyForPrune (hasEnmity outer gate two-bool AND)
//   - residual 2807: hasEnmity stale-mob prune gates
//     (ShouldScanNotorietyForPrune dual-wired as 3020; ShouldPruneMobFromNotoriety residual)
//   - residual 2818: add admission (prior pure port of ShouldAddNotorietyMember)
//   - residual 2819: remove admission (prior pure port of ShouldRemoveNotorietyMember)
//   - residual 2832: hasEnmity / size pure reporting
//
// Production host: CNotorietyContainer in notoriety_container.cpp.
// Helpers take host-injected scalars/bools only (no entity/enmity pointers).
// Go dual-wire: notoriety.ShouldAddNotorietyMember
// (internal/notoriety/add_member.go). Prior pure port: slice 2818.
// Go dual-wire: notoriety.ShouldRemoveNotorietyMember
// (internal/notoriety/remove_member.go). Prior pure port: slice 2819.
// Go dual-wire: notoriety.ShouldScanNotorietyForPrune
// (internal/notoriety/scan_prune.go). Prior pure port: slice 2807.

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
// hasEnmity walk after dynamic_cast<CMobEntity*>:
//
//   if !isMob → never prune in this loop (non-mobs retained)
//   if isMob  → (isAlive && notOnEnmityList) || isDead
//
// isAlive and isDead are injected separately as production does; both may be
// false for a transitional host. notOnEnmityList is host-computed as
// enmity list missing owner id (as uint16).
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
// Formula (slice 2959 dual-wire):
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
// Formula (slice 2971 dual-wire):
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
// Call site: CNotorietyContainer::remove (notoriety_container.cpp).
inline auto ShouldRemoveNotorietyMember(const bool ownerPresent, const bool entityPresent) -> bool
{
    return ownerPresent && entityPresent;
}

// HasEnmityAfterPrune mirrors CNotorietyContainer::hasEnmity final return (~110)
// after the optional stale-mob prune walk:
//   !m_Lookup.empty()
//
// Host injects empty-state only (no set pointers). True when the reverse list
// still has at least one entry after pruning (or when the prune walk was
// skipped and the lookup was already non-empty).
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
