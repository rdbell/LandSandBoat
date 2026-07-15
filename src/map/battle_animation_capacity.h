#pragma once

#include <cstdint>

// Pure CBattleEntity::isMounted / isSitting with fully injected animation.
// Parity: internal/battleanimation (slice 1658).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::isMounted (~255–258)
//   CBattleEntity::isSitting (~260–263)
//
// Host retains animation storage; helpers take the injected value only.
// ANIMATIONTYPE pins are defined here for independence from other capacity
// headers (entity_action_capacity, etc.).

namespace battleanimationhelpers
{

// ANIMATIONTYPE pins (base_entity.h) used by isMounted / isSitting.
constexpr std::uint8_t AnimationChocobo    = 5;  // ANIMATION_CHOCOBO
constexpr std::uint8_t AnimationHealing    = 33; // ANIMATION_HEALING
constexpr std::uint8_t AnimationSit        = 47; // ANIMATION_SIT
constexpr std::uint8_t AnimationSitchair0  = 63; // ANIMATION_SITCHAIR_0
constexpr std::uint8_t AnimationSitchair1  = 64; // ANIMATION_SITCHAIR_1
constexpr std::uint8_t AnimationSitchair2  = 65; // ANIMATION_SITCHAIR_2
constexpr std::uint8_t AnimationSitchair3  = 66; // ANIMATION_SITCHAIR_3
constexpr std::uint8_t AnimationSitchair4  = 67; // ANIMATION_SITCHAIR_4
constexpr std::uint8_t AnimationSitchair5  = 68; // ANIMATION_SITCHAIR_5
constexpr std::uint8_t AnimationSitchair6  = 69; // ANIMATION_SITCHAIR_6
constexpr std::uint8_t AnimationSitchair7  = 70; // ANIMATION_SITCHAIR_7
constexpr std::uint8_t AnimationSitchair8  = 71; // ANIMATION_SITCHAIR_8
constexpr std::uint8_t AnimationSitchair9  = 72; // ANIMATION_SITCHAIR_9
constexpr std::uint8_t AnimationSitchair10 = 73; // ANIMATION_SITCHAIR_10
constexpr std::uint8_t AnimationMount      = 85; // ANIMATION_MOUNT

// IsMounted mirrors CBattleEntity::isMounted:
//   animation == ANIMATION_CHOCOBO || animation == ANIMATION_MOUNT
inline auto IsMounted(const std::uint8_t animation) -> bool
{
    return animation == AnimationChocobo || animation == AnimationMount;
}

// IsSitting mirrors CBattleEntity::isSitting:
//   ANIMATION_HEALING || ANIMATION_SIT ||
//   (ANIMATION_SITCHAIR_0 .. ANIMATION_SITCHAIR_10 inclusive)
inline auto IsSitting(const std::uint8_t animation) -> bool
{
    return animation == AnimationHealing ||
           animation == AnimationSit ||
           (animation >= AnimationSitchair0 && animation <= AnimationSitchair10);
}

} // namespace battleanimationhelpers
