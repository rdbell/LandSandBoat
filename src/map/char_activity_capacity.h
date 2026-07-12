#pragma once

#include <cstdint>

// Pure isCrafting / isFishing predicates from CCharEntity.

namespace charactivityhelpers
{

// Animation pins from base_entity.h.
constexpr std::uint8_t AnimationSynth           = 44;
constexpr std::uint8_t AnimationFishingStartOld = 50;
constexpr std::uint8_t AnimationFishingStart    = 56;
constexpr std::uint8_t AnimationFishingFish     = 57;
constexpr std::uint8_t AnimationFishingStop     = 62;

// IsCrafting mirrors animation == ANIMATION_SYNTH || active SynthTransaction.
constexpr auto IsCrafting(const std::uint8_t animation, const bool hasSynthTransaction) -> bool
{
    return animation == AnimationSynth || hasSynthTransaction;
}

// IsFishing mirrors:
//   (animation >= FISHING_FISH && animation <= FISHING_STOP) ||
//   animation == FISHING_START_OLD || animation == FISHING_START
constexpr auto IsFishing(const std::uint8_t animation) -> bool
{
    return (animation >= AnimationFishingFish && animation <= AnimationFishingStop) ||
           animation == AnimationFishingStartOld ||
           animation == AnimationFishingStart;
}

} // namespace charactivityhelpers
