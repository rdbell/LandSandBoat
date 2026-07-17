#pragma once

// Pure CNotorietyContainer::hasEnmity stale-mob prune policy (slice 2807).
//
// Production host: CNotorietyContainer::hasEnmity in notoriety_container.cpp.
// Helpers take host-injected bools only (no entity/enmity pointers).

namespace notorietyhelpers
{

// ShouldScanNotorietyForPrune mirrors the hasEnmity outer gate:
//   m_POwner && !m_Lookup.empty()
// When false, the host skips the prune walk and reports !lookup.empty().
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
// Host injects presence and the allegiance-inequality bool only (no entity
// pointers). differentAllegiance is host-safe: false when either pointer is
// null, otherwise entity->allegiance != m_POwner->allegiance.
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
// Host injects presence bools only (no entity pointers). When true, the host
// may find/erase the entity from m_Lookup; when false, remove is a no-op.
inline auto ShouldRemoveNotorietyMember(const bool ownerPresent, const bool entityPresent) -> bool
{
    return ownerPresent && entityPresent;
}

} // namespace notorietyhelpers
