#pragma once

// Pure CItemState::Update phase gates (slice 6316).
// Dual-wire of Go aistate.ItemCastComplete / ItemShouldExit
// (internal/aistate/item.go). Host owns InterruptItem/FinishItem/packets.

namespace itemupdate
{

// shouldFinishCast mirrors the cast-complete branch of CItemState::Update:
//   tick > GetEntryTime() + m_castTime && !IsCompleted()
// tickAfterEntryPlusCast — host tick > entry + castTime (strict >)
// completed — host IsCompleted()
constexpr auto shouldFinishCast(const bool tickAfterEntryPlusCast, const bool completed) -> bool
{
    return !completed && tickAfterEntryPlusCast;
}

// shouldExit mirrors the post-animation exit branch of CItemState::Update:
//   IsCompleted() && tick > GetEntryTime() + m_castTime + m_animationTime
// tickAfterEntryPlusCastPlusAnim — host tick > entry + cast + animation (strict >)
// completed — host IsCompleted()
constexpr auto shouldExit(const bool tickAfterEntryPlusCastPlusAnim, const bool completed) -> bool
{
    return completed && tickAfterEntryPlusCastPlusAnim;
}

} // namespace itemupdate
