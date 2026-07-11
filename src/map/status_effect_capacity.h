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


// --- Slice 1366: expiry, tick due, aura range, eleven-roll, perpetuation ---

// Default aura base radius in yalms before AURA_SIZE mod.
constexpr float AuraBaseRange = 6.0f;

// Aura effect create duration/tick mirrors HandleAura (3s tick, 4s duration).
constexpr uint32 AuraEffectTickSeconds     = 3;
constexpr uint32 AuraEffectDurationSeconds = 4;

// Corsair roll ID range for eleven-roll (FightersRoll..NaturalistsRoll).
// FightersRoll=310 .. NaturalistsRoll=339; RuneistsRoll=600.
constexpr uint16 ElevenRollIDFirst  = 310;
constexpr uint16 ElevenRollIDLast   = 339;
constexpr uint16 RuneistsRollID     = 600;
constexpr uint16 ElevenRollSubPower = 11;

// Nightmare sleep tier threshold (tier >= 4 is player avatar Nightmare).
constexpr uint8 NightmareSleepTierMin = 4;

// Avatar Favor perpetuation multiplier numerator/denominator for floor(cost * 1.2).
// Implemented as cost * 6 / 5 after reductions when Favor applies.
// Production: static_cast<int16>(perpetuationCost * 1.2) which floors.

// ShouldExpireEffect mirrors duration != 0 && start+duration <= tick.
// Times are host-normalized units (same scale).
inline auto ShouldExpireEffect(
    const bool durationNonzero,
    const int64 expiryTime,
    const int64 tickTime) -> bool
{
    return durationNonzero && expiryTime <= tickTime;
}

// ShouldTickEffect mirrors tickTime != 0 && elapsedTicks < (tick-start)/tickPeriod.
// elapsedThreshold is host-computed (tick - start) / tickPeriod as integer division.
inline auto ShouldTickEffect(const bool tickPeriodNonzero, const uint32 elapsedTickCount, const uint32 elapsedThreshold) -> bool
{
    return tickPeriodNonzero && elapsedTickCount < elapsedThreshold;
}

// ComputeAuraRange mirrors 6.0 + (AURA_SIZE / 100.0).
inline auto ComputeAuraRange(const int16 auraSizeMod) -> float
{
    return AuraBaseRange + (static_cast<float>(auraSizeMod) / 100.0f);
}

// IsWithinAuraRange mirrors distance <= auraRange + modelHitboxSize.
inline auto IsWithinAuraRange(const float distance, const float auraRange, const float modelHitboxSize) -> bool
{
    return distance <= auraRange + modelHitboxSize;
}

// ShouldUseMasterForAura mirrors pet or trust owner redirection.
inline auto ShouldUseMasterForAura(const bool isPet, const bool isTrust) -> bool
{
    return isPet || isTrust;
}

// ResolveAuraEffectIcon mirrors subIcon > 0 ? subIcon : subID.
inline auto ResolveAuraEffectIcon(const uint16 subIcon, const uint16 subID) -> uint16
{
    return subIcon > 0 ? subIcon : subID;
}

// ShouldRefreshAlwaysExpiringAura mirrors existing effect with AlwaysExpiring flag.
inline auto ShouldRefreshAlwaysExpiringAura(const bool hasEffect, const bool hasAlwaysExpiringFlag) -> bool
{
    return hasEffect && hasAlwaysExpiringFlag;
}

// ShouldUpdateAuraPower mirrors power != subPower on refresh path.
inline auto ShouldUpdateAuraPower(const uint16 existingPower, const uint16 auraSubPower) -> bool
{
    return existingPower != auraSubPower;
}

// IsElevenRollEffect mirrors FightersRoll..NaturalistsRoll or RuneistsRoll with subPower 11.
inline auto IsElevenRollEffect(const uint16 statusID, const uint16 subPower, const uint16 firstRoll, const uint16 lastRoll, const uint16 runeistsRoll) -> bool
{
    if (subPower != ElevenRollSubPower)
    {
        return false;
    }
    if (statusID >= firstRoll && statusID <= lastRoll)
    {
        return true;
    }
    return statusID == runeistsRoll;
}

// ShouldBreakSleepFromRegenDown mirrors NOT (has SleepI && tier >= 4).
inline auto ShouldBreakSleepFromRegenDown(const bool hasSleepI, const uint8 sleepTier) -> bool
{
    return !(hasSleepI && sleepTier >= NightmareSleepTierMin);
}

// ShouldApplyRegainTP mirrors objtype != MOB || engaged.
inline auto ShouldApplyRegainTP(const bool isMob, const bool isEngaged) -> bool
{
    return !isMob || isEngaged;
}

// ShouldDespawnAvatarOnZeroMP mirrors mp==0 && has pet && pet is avatar type.
inline auto ShouldDespawnAvatarOnZeroMP(const bool mpIsZero, const bool hasPet, const bool petIsAvatar) -> bool
{
    return mpIsZero && hasPet && petIsAvatar;
}

// HalfPerpetuationCost floors cost/2 when any half flag applies.
inline auto ApplyHalfPerpetuation(const int16 cost, const bool applyHalf) -> int16
{
    return applyHalf ? static_cast<int16>(cost / 2) : cost;
}

// AdjustPerpetuationAfterHalf applies reduction, elemental, day, weather subtracts.
inline auto AdjustPerpetuationAfterHalf(
    const int16 halfAdjustedCost,
    const int16 perpetuationReduction,
    const int16 elementalAffinityPerp,
    const int16 dayReduction,
    const int16 weatherReduction) -> int16
{
    return static_cast<int16>(
        halfAdjustedCost - perpetuationReduction - elementalAffinityPerp - dayReduction - weatherReduction);
}

// ApplyAvatarFavorPerpetuation floors cost * 1.2 via double cast parity.
inline auto ApplyAvatarFavorPerpetuation(const int16 cost, const bool applyFavor) -> int16
{
    if (!applyFavor)
    {
        return cost;
    }
    return static_cast<int16>(cost * 1.2);
}

// FinalizePerpetuationCost: Astral Flow zeros; else clamp min 1.
inline auto FinalizePerpetuationCost(const int16 cost, const bool hasAstralFlow) -> int16
{
    if (hasAstralFlow)
    {
        return 0;
    }
    return cost < 1 ? static_cast<int16>(1) : cost;
}

// ShouldApplyAvatarPerpetuationPath mirrors AVATAR_PERPETUATION > 0 && TYPE_PC.
inline auto ShouldApplyAvatarPerpetuationPath(const int16 avatarPerpetuationMod, const bool isPC) -> bool
{
    return avatarPerpetuationMod > 0 && isPC;
}

// IsPetElementValid mirrors element in [FIRE..DARK] (1..8 typically).
inline auto IsPetElementValid(const uint8 petElement, const uint8 elementFire, const uint8 elementDark) -> bool
{
    return petElement >= elementFire && petElement <= elementDark;
}

// WeatherMatchesPetStrong mirrors weather == strong || weather == strong+1 (double weather).
inline auto WeatherMatchesPetStrong(const uint16 weather, const uint16 weatherStrong) -> bool
{
    return weather == weatherStrong || weather == static_cast<uint16>(weatherStrong + 1);
}


} // namespace statuseffecthelpers
