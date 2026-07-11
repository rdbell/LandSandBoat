#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
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

// --- Slice 1365: erase/dispel/waltz, bard song apply, highest-rune pure halves ---

// Bard song inclusive ID range (Requiem..Nocturne).
constexpr uint16 BardSongIDFirst = 192;
constexpr uint16 BardSongIDLast  = 223;

// Rune effect inclusive ID range (Ignis..Tenebrae).
constexpr uint16 RuneIDFirst = 523;
constexpr uint16 RuneIDLast  = 530;

// Maneuver inclusive ID range (FireManeuver..DarkManeuver).
constexpr uint16 ManeuverIDFirst = 300;
constexpr uint16 ManeuverIDLast  = 307;

// IsTimedActiveCandidate mirrors duration > 0 && !deleted for removable lists.
inline auto IsTimedActiveCandidate(const bool durationPositive, const bool deleted) -> bool
{
    return durationPositive && !deleted;
}

// IsErasableCandidate mirrors EraseStatusEffect filter.
inline auto IsErasableCandidate(const bool hasErasable, const bool durationPositive, const bool deleted) -> bool
{
    return hasErasable && IsTimedActiveCandidate(durationPositive, deleted);
}

// IsWaltzableCandidate mirrors HealingWaltz filter (waltzable || erasable).
inline auto IsWaltzableCandidate(
    const bool hasWaltzable,
    const bool hasErasable,
    const bool durationPositive,
    const bool deleted) -> bool
{
    return (hasWaltzable || hasErasable) && IsTimedActiveCandidate(durationPositive, deleted);
}

// IsFlagRemovableCandidate mirrors DispelStatusEffect / DispelAll filter.
inline auto IsFlagRemovableCandidate(
    const bool hasFlag,
    const bool durationPositive,
    const bool deleted) -> bool
{
    return hasFlag && IsTimedActiveCandidate(durationPositive, deleted);
}

// HasRemovableCandidates gates random pick when list non-empty.
inline auto HasRemovableCandidates(const std::size_t count) -> bool
{
    return count > 0;
}

// IsInInclusiveIDRange mirrors start <= id <= end.
inline auto IsInInclusiveIDRange(const uint16 id, const uint16 first, const uint16 last) -> bool
{
    return id >= first && id <= last;
}

// IsBardSongID mirrors Requiem..Nocturne check.
inline auto IsBardSongID(const uint16 id) -> bool
{
    return IsInInclusiveIDRange(id, BardSongIDFirst, BardSongIDLast);
}

// IsRuneID mirrors Ignis..Tenebrae check.
inline auto IsRuneID(const uint16 id) -> bool
{
    return IsInInclusiveIDRange(id, RuneIDFirst, RuneIDLast);
}

// IsManeuverID mirrors FireManeuver..DarkManeuver check.
inline auto IsManeuverID(const uint16 id) -> bool
{
    return IsInInclusiveIDRange(id, ManeuverIDFirst, ManeuverIDLast);
}

// IsSameBardSongOverwrite mirrors same tier && same status ID.
inline auto IsSameBardSongOverwrite(
    const uint8 newTier,
    const uint16 newID,
    const uint8 existingTier,
    const uint16 existingID) -> bool
{
    return existingTier == newTier && existingID == newID;
}

// IsOwnBardSong mirrors ExistingStatusEffect->GetSubID() == PStatusEffect->GetSubID().
inline auto IsOwnBardSong(const uint32 existingSubID, const uint32 newSubID) -> bool
{
    return existingSubID == newSubID;
}

// IsEarlierSongExpiry mirrors start+duration comparison for oldest own song.
// Times are host-normalized monotonic units (e.g. milliseconds since epoch).
inline auto IsEarlierSongExpiry(
    const int64 candidateExpiry,
    const int64 oldestExpiry) -> bool
{
    return candidateExpiry < oldestExpiry;
}

// CanApplyBardWithoutReplace mirrors numOfEffects < maxSongs.
inline auto CanApplyBardWithoutReplace(const uint8 numOfEffects, const uint8 maxSongs) -> bool
{
    return numOfEffects < maxSongs;
}

// ShouldAssignLowestFreeSlot mirrors GetEffectSlot() == 0 on apply path.
inline auto ShouldAssignLowestFreeSlot(const uint8 effectSlot) -> bool
{
    return effectSlot == 0;
}

// CanApplyBardReplacingOldest mirrors else-if (oldestSong) after full song slots.
inline auto CanApplyBardReplacingOldest(const bool hasOldestSong) -> bool
{
    return hasOldestSong;
}

// ShouldCountActiveInRange mirrors id in [first,last] && !deleted.
inline auto ShouldCountActiveInRange(
    const uint16 id,
    const uint16 first,
    const uint16 last,
    const bool deleted) -> bool
{
    return IsInInclusiveIDRange(id, first, last) && !deleted;
}

// PreferHigherRuneCount: true when candidate should replace current highest.
// Ties keep the current (first-seen) highest — matches iter->second > highestRuneValue.
inline auto PreferHigherRuneCount(
    const bool hasCurrent,
    const uint8 currentCount,
    const uint8 candidateCount) -> bool
{
    return !hasCurrent || candidateCount > currentCount;
}

// ShouldRemoveAllInRange mirrors RemoveAllStatusEffectsInIDRange id check
// (note: production does not skip deleted here).
inline auto ShouldRemoveAllInRange(const uint16 id, const uint16 first, const uint16 last) -> bool
{
    return IsInInclusiveIDRange(id, first, last);
}


} // namespace statuseffecthelpers
