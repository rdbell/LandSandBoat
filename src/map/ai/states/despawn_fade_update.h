#pragma once

#include <cstdint>

// Pure CDespawnState FadeOut ctor + Update complete gates (slice 6299).
//
// Production (despawn_state.cpp):
//   Ctor FadeOut SCHEDULOR:
//     if shouldPushFadeOut(instant, status, behavior):
//         zone->PushPacket(... FourCC::FadeOut)   // host
//   Update before OnDespawn+Complete:
//     if shouldComplete(IsCompleted(), behavior, tick >= despawnTime_):
//         OnDespawn(*this); Complete();           // host
//     return IsCompleted();
//
// Mirrors OmegaXI internal/aistate despawn.go (0770 pure half):
//   ShouldPushDespawnFadeOut / ShouldProcessDespawnUpdate /
//   DespawnReady / DespawnShouldComplete.
//
// Packet bytes, OnDespawn/FadeOut body, respawn registration (6287),
// Internal_Despawn attachment, and full PAI ChangeState remain host-owned.

namespace despawnfadeupdate
{
// STATUS_TYPE::DISAPPEAR (base_entity.h)
constexpr uint8_t kStatusDisappear = 2;
// BEHAVIOR_NO_DESPAWN (mob_entity.h)
constexpr uint16_t kBehaviorNoDespawn = 0x001;

constexpr auto hasBehaviorNoDespawn(const uint16_t behavior) -> bool
{
    return (behavior & kBehaviorNoDespawn) != 0;
}

// Ctor FadeOut SCHEDULOR predicate:
//   !instantDespawn && status != DISAPPEAR && !(behavior & BEHAVIOR_NO_DESPAWN)
constexpr auto shouldPushFadeOut(const bool instantDespawn, const uint8_t status, const uint16_t behavior) -> bool
{
    return !instantDespawn && status != kStatusDisappear && !hasBehaviorNoDespawn(behavior);
}

// Outer Update gate: !IsCompleted() && !(behavior & BEHAVIOR_NO_DESPAWN)
constexpr auto shouldProcessUpdate(const bool completed, const uint16_t behavior) -> bool
{
    return !completed && !hasBehaviorNoDespawn(behavior);
}

// Inner ready: tick >= despawnTime_ (host precomputes tickAtOrAfterDespawnTime).
constexpr auto ready(const bool tickAtOrAfterDespawnTime) -> bool
{
    return tickAtOrAfterDespawnTime;
}

// Combined completion predicate before OnDespawn + Complete.
constexpr auto shouldComplete(const bool completed, const uint16_t behavior, const bool tickAtOrAfterDespawnTime) -> bool
{
    return shouldProcessUpdate(completed, behavior) && ready(tickAtOrAfterDespawnTime);
}
} // namespace despawnfadeupdate
