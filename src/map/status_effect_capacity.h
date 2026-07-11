#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CStatusEffectContainer::CanGainStatusEffect policy halves.

namespace statuseffecthelpers
{

// EffectOverwrite numeric mirrors (data/enums/effect_overwrite.yaml).
// Values match xi::EffectOverwrite (EqualHigher=0 … TierHigher=5).
constexpr uint8 OverwriteEqualHigher     = 0;
constexpr uint8 OverwriteHigher          = 1;
constexpr uint8 OverwriteNever           = 2;
constexpr uint8 OverwriteAlways          = 3;
constexpr uint8 OverwriteIgnoreDuplicate = 4;
constexpr uint8 OverwriteTierHigher      = 5;

// Aftermath powers that block spikes (Geirskogul edge case).
constexpr uint16 AftermathPowerSpikesBlockA = 8;
constexpr uint16 AftermathPowerSpikesBlockB = 22;

// ShouldBlockSpikesDueToAftermath mirrors aftermath power 8 or 22.
inline auto ShouldBlockSpikesDueToAftermath(const bool hasAftermath, const uint16 aftermathPower) -> bool
{
    return hasAftermath && (aftermathPower == AftermathPowerSpikesBlockA || aftermathPower == AftermathPowerSpikesBlockB);
}

// ShouldBlockCharmOnPet mirrors charm effect && PMaster != nullptr.
inline auto ShouldBlockCharmOnPet(const bool isCharmEffect, const bool hasMaster) -> bool
{
    return isCharmEffect && hasMaster;
}

// ShouldBlockByBlockId mirrors blockId != 0 && HasStatusEffect(blockId).
inline auto ShouldBlockByBlockId(const uint16 blockId, const bool hasBlockEffect) -> bool
{
    return blockId != 0 && hasBlockEffect;
}

// IsHasteVsSlowRemote mirrors Haste vs Slow with subPower == 1 (remote slow).
inline auto IsHasteVsSlowRemote(
    const bool isHaste,
    const bool negativeIsSlow,
    const uint16 negativeSubPower) -> bool
{
    return isHaste && negativeIsSlow && negativeSubPower == 1;
}

// CompareTiersOrPower: when both tiers nonzero use tier op, else power op.
// preferEqual includes equality ( >= / <= ); higherOnly is strict >.
inline auto CompareTiersOrPower(
    const uint8  newTier,
    const uint16 newPower,
    const uint8  otherTier,
    const uint16 otherPower,
    const bool   preferEqual) -> bool
{
    if (newTier != 0 && otherTier != 0)
    {
        return preferEqual ? newTier >= otherTier : newTier > otherTier;
    }
    return preferEqual ? newPower >= otherPower : newPower > otherPower;
}

// CanGainVsNegative mirrors negative-effect competition after remote-haste special case.
// returns true if new effect is allowed against the negative.
// useTierCompare when both tiers nonzero; otherwise power (always preferEqual for negative).
// For tier path when both nonzero: equal tiers → statusEffect > negativeId (caller injects statusGreaterThanNegative).
inline auto CanGainVsNegative(
    const uint8  newTier,
    const uint16 newPower,
    const uint8  negativeTier,
    const uint16 negativePower,
    const bool   bothTiersNonzero,
    const bool   statusGreaterThanNegative) -> bool
{
    if (bothTiersNonzero)
    {
        if (newTier != negativeTier)
        {
            return newTier > negativeTier;
        }
        return statusGreaterThanNegative;
    }
    return newPower >= negativePower;
}

// CanGainOverwrite mirrors the overwrite switch when an existing same-ID effect is present.
// overwrite is xi::EffectOverwrite underlying value.
inline auto CanGainOverwrite(
    const uint8  overwrite,
    const uint8  newTier,
    const uint16 newPower,
    const uint8  existingTier,
    const uint16 existingPower) -> bool
{
    if (overwrite == OverwriteAlways || overwrite == OverwriteIgnoreDuplicate)
    {
        return true;
    }
    if (overwrite == OverwriteNever)
    {
        return false;
    }
    if (overwrite == OverwriteEqualHigher)
    {
        return CompareTiersOrPower(newTier, newPower, existingTier, existingPower, true);
    }
    if (overwrite == OverwriteHigher)
    {
        return CompareTiersOrPower(newTier, newPower, existingTier, existingPower, false);
    }
    if (overwrite == OverwriteTierHigher)
    {
        if (newTier != 0 && existingTier != 0)
        {
            return newTier > existingTier;
        }
        return false;
    }
    return false;
}

// ShouldDeleteOnOverwrite mirrors overwrite != IgnoreDuplicate in OverwriteStatusEffect.
inline auto ShouldDeleteOnOverwrite(const uint8 overwrite) -> bool
{
    return overwrite != OverwriteIgnoreDuplicate;
}

// ShouldRemoveLinkedId mirrors removeId > Ko (status effect 0 is KO).
// removeId is the catalog RemoveId numeric.
inline auto ShouldRemoveLinkedId(const uint16 removeId, const uint16 koId) -> bool
{
    return removeId > koId;
}

// Sleep subpower element injects for immunity branching.
// lightElement/darkElement are battleutils ELEMENT_* values from host.
inline auto IsLightSleepSubPower(const uint16 subPower, const uint16 lightElement) -> bool
{
    return subPower == lightElement;
}

inline auto IsDarkSleepSubPower(const uint16 subPower, const uint16 darkElement) -> bool
{
    return subPower == darkElement;
}

// ShouldRejectSleepImmunity mirrors light/dark sleep immunity for sleep family.
inline auto ShouldRejectSleepImmunity(
    const bool isSleepFamily,
    const bool isLightSub,
    const bool isDarkSub,
    const bool immuneLightSleep,
    const bool immuneDarkSleep) -> bool
{
    if (!isSleepFamily)
    {
        return false;
    }
    if (isLightSub && immuneLightSleep)
    {
        return true;
    }
    if (isDarkSub && immuneDarkSleep)
    {
        return true;
    }
    return false;
}

// ShouldRejectSimpleImmunity mirrors single-flag immunity for one status.
inline auto ShouldRejectSimpleImmunity(const bool hasImmunity) -> bool
{
    return hasImmunity;
}

// CanGainWhenNoExisting returns true (default allow when no same-id effect).
inline auto CanGainWhenNoExisting() -> bool
{
    return true;
}

// HasNegativeEffect gates the negative comparison path.
inline auto HasNegativeEffect(const uint16 negativeId, const bool negativePresent) -> bool
{
    return negativeId != 0 && negativePresent;
}

// HasExistingEffect gates the overwrite path.
inline auto HasExistingEffect(const bool existingPresent) -> bool
{
    return existingPresent;
}

} // namespace statuseffecthelpers
