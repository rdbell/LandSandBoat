#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

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

// --- Slice 3100: ShouldBlockCharmOnPet pure dual-wire ---
// Residual pure port: slice 1364 (CanGain overwrite / negative / immunity suite).
// Production host: CStatusEffectContainer::CanGainStatusEffect injects
// (statusEffect == CharmI || CharmIi) and (m_POwner->PMaster != nullptr) into
// ShouldBlockCharmOnPet; on true return false (pets cannot be charmed).
// Go dual-wire: statuseffect.ShouldBlockCharmOnPet
// (internal/statuseffect/block_charm_on_pet.go).
// Sibling dual-wires 3049 / 3069 / 3080 (expire / tick / null-add) left alone.
// Index 3100: statuseffect.ShouldBlockCharmOnPet pure dual-wire.

// ShouldBlockCharmOnPet mirrors charm effect && PMaster != nullptr.
//
// Formula (slice 3100 dual-wire):
//   isCharmEffect && hasMaster
//
// isCharmEffect — host-injected (status is CharmI or CharmIi)
// hasMaster     — host-injected (m_POwner->PMaster != nullptr)
// true  → host rejects CanGainStatusEffect (pets cannot be charmed)
// false → proceed past charm-on-pet gate
//
// Dual-wire of Go statuseffect.ShouldBlockCharmOnPet.
// Call site: CStatusEffectContainer::CanGainStatusEffect — host injects
// CharmI/CharmIi membership and PMaster != nullptr; on true return false.
// Prior pure port: slice 1364 (can-gain pure policy suite).
// Residual pins remain in test_status_effect_can_gain_1364; dedicated
// dual-wire suite is test_status_block_charm_pet_3100.
// Sibling dual-wires: ShouldExpireEffect (3049) / ShouldTickEffect (3069) /
// ShouldRejectNullStatusEffect (3080) are orthogonal.
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

// --- Slice 3113: ShouldRejectSimpleImmunity pure dual-wire ---
// Residual pure port: slice 1364 (CanGain overwrite / negative / immunity suite).
// Production host: CStatusEffectContainer::CanGainStatusEffect injects
// m_POwner->hasImmunity(IMMUNITY_*) for Weight/Bind/Stun/Silence/Paralysis/
// Blindness/Slow/Poison/Elegy/Requiem/Terror/Petrification into
// ShouldRejectSimpleImmunity; on true return false.
// Go dual-wire: statuseffect.ShouldRejectSimpleImmunity
// (internal/statuseffect/reject_simple_immunity.go).
// Sibling dual-wires 3049 / 3069 / 3080 / 3100 (expire / tick / null-add /
// charm-on-pet) left alone.
// Index 3113: statuseffect.ShouldRejectSimpleImmunity pure dual-wire.

// ShouldRejectSimpleImmunity mirrors single-flag immunity for one status.
//
// Formula (slice 3113 dual-wire):
//   hasImmunity
//
// hasImmunity — host-injected (m_POwner->hasImmunity(IMMUNITY_*))
// true  → host rejects CanGainStatusEffect for that status
// false → proceed past simple-immunity gate
//
// Dual-wire of Go statuseffect.ShouldRejectSimpleImmunity.
// Call site: CStatusEffectContainer::CanGainStatusEffect — host injects
// hasImmunity for single-flag statuses; on true return false.
// Prior pure port: slice 1364 (can-gain pure policy suite).
// Residual pins remain in test_status_effect_can_gain_1364; dedicated
// dual-wire suite is test_status_reject_simple_immunity_3113.
// Sibling dual-wires: ShouldExpireEffect (3049) / ShouldTickEffect (3069) /
// ShouldRejectNullStatusEffect (3080) / ShouldBlockCharmOnPet (3100) are
// orthogonal.
inline auto ShouldRejectSimpleImmunity(const bool hasImmunity) -> bool
{
    return hasImmunity;
}

// --- Slice 3135: CanGainWhenNoExisting pure dual-wire ---
// Residual pure port: slice 1364 (CanGain overwrite / negative / immunity suite).
// Production host: CStatusEffectContainer::CanGainStatusEffect returns
// CanGainWhenNoExisting() when GetStatusEffect(statusId) is null (no same-ID
// existing effect); default allow (true).
// Go dual-wire: statuseffect.CanGainWhenNoExisting
// (internal/statuseffect/can_gain_when_no_existing.go).
// Sibling dual-wires 3049 / 3069 / 3080 / 3100 / 3113 (expire / tick /
// null-add / charm-on-pet / simple-immunity) left alone.
// Index 3135: statuseffect.CanGainWhenNoExisting pure dual-wire.

// CanGainWhenNoExisting returns true when no same-ID effect is present.
//
// Formula (slice 3135 dual-wire):
//   true
//
// Host reaches this residual only after GetStatusEffect(statusId) == nullptr
// (no same-ID existing effect). Default allow for CanGainStatusEffect.
//
// Dual-wire of Go statuseffect.CanGainWhenNoExisting.
// Call site: CStatusEffectContainer::CanGainStatusEffect — after existing
// same-ID lookup is null, return CanGainWhenNoExisting() (true).
// Prior pure port: slice 1364 (can-gain pure policy suite).
// Residual pins remain in test_status_effect_can_gain_1364; dedicated
// dual-wire suite is test_status_can_gain_no_existing_3135.
// Sibling dual-wires: ShouldExpireEffect (3049) / ShouldTickEffect (3069) /
// ShouldRejectNullStatusEffect (3080) / ShouldBlockCharmOnPet (3100) /
// ShouldRejectSimpleImmunity (3113) are orthogonal.
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

// ShouldSelectOlderInRange mirrors the candidate gate and strict start-time
// comparison used by RemoveOldestStatusEffectInIDRange. Equal start times keep
// the first candidate encountered by the container traversal.
inline auto ShouldSelectOlderInRange(
    const uint16 id,
    const uint16 first,
    const uint16 last,
    const bool deleted,
    const bool hasCurrent,
    const int64 candidateStart,
    const int64 currentStart) -> bool
{
    return IsInInclusiveIDRange(id, first, last) && !deleted && (!hasCurrent || candidateStart < currentStart);
}

// ShouldSelectNewerInRange mirrors the candidate gate and strict start-time
// comparison used by RemoveNewestStatusEffectInIDRange. Equal start times
// keep the first candidate encountered by the container traversal.
inline auto ShouldSelectNewerInRange(
    const uint16 id,
    const uint16 first,
    const uint16 last,
    const bool deleted,
    const bool hasCurrent,
    const int64 candidateStart,
    const int64 currentStart) -> bool
{
    return IsInInclusiveIDRange(id, first, last) && !deleted && (!hasCurrent || candidateStart > currentStart);
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
//
// Formula (slice 3225 dedicated dual-wire expand residual 3049; pure 1366 —
// formula unchanged):
//   durationNonzero && expiryTime <= tickTime
//
// durationNonzero — host-evaluated GetDuration() != 0
// expiryTime      — host-normalized start+duration (or equivalent expiry)
// tickTime        — host-normalized current tick time
// true  → effect expires this tick (host RemoveStatusEffect)
// false → keep effect
//
// Times are host-normalized units on the same scale (production uses
// time_since_epoch counts from timer::time_point).
//
// Dual-wire of Go statuseffect.ShouldExpireEffect.
// Call site: CStatusEffectContainer::CheckEffectsExpiry — host injects
// duration != 0s and (start+duration).count() / tick.count(); on true
// RemoveStatusEffect.
// Prior pure port: slice 1366 (expiry / tick / aura / eleven-roll suite).
// Residual pins remain in test_status_effect_tick_1366; residual dual-wire
// suite is test_status_expire_effect_3049. Dedicated expand residual suite is
// test_statuseffect_expire_effect_3225. Sibling dual-wire: ShouldTickEffect
// (tick-period due; slice 3069) is orthogonal.
// Index 3049: statuseffect.ShouldExpireEffect pure dual-wire (residual).
// Index 3225: statuseffect.ShouldExpireEffect dedicated dual-wire expand
// residual 3049 (formula unchanged).
inline auto ShouldExpireEffect(
    const bool durationNonzero,
    const int64 expiryTime,
    const int64 tickTime) -> bool
{
    return durationNonzero && expiryTime <= tickTime;
}

// ShouldTickEffect mirrors tickPeriod != 0 && elapsedTicks < (tick-start)/tickPeriod.
//
// Formula (slice 3069 dual-wire):
//   tickPeriodNonzero && elapsedTickCount < elapsedThreshold
//
// tickPeriodNonzero  — host-evaluated GetTickTime() != 0
// elapsedTickCount   — host-supplied GetElapsedTickCount()
// elapsedThreshold   — host-computed (tick - start) / tickPeriod as integer division
// true  → effect is due this tick (host OnEffectTick / IncrementElapsedTickCount)
// false → skip effect this tick
//
// elapsedThreshold is 0 when tickPeriod is zero (host short-circuits the
// division); with tickPeriodNonzero false the gate is always false.
//
// Dual-wire of Go statuseffect.ShouldTickEffect.
// Call site: CStatusEffectContainer::TickEffects — host injects
// tickPeriod != 0s, GetElapsedTickCount(), and (tick-start)/tickPeriod;
// on true HandleAura (if Aura flag) / IncrementElapsedTickCount / OnEffectTick.
// Prior pure port: slice 1366 (expiry / tick / aura / eleven-roll suite).
// Residual pins remain in test_status_effect_tick_1366; dedicated dual-wire
// suite is test_status_tick_effect_3069. Sibling dual-wire: ShouldExpireEffect
// (slice 3049) is orthogonal.
// Index 3069: statuseffect.ShouldTickEffect pure dual-wire.
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

// ResolveAuraSubIcon mirrors new-effect path icon selection in HandleAura
// (subIcon > 0 ? subIcon : subID). Same pure rule as ResolveAuraEffectIcon.
inline auto ResolveAuraSubIcon(const uint16 subIcon, const uint16 subID) -> uint16
{
    return ResolveAuraEffectIcon(subIcon, subID);
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

// --- Slice 2798: HandleAura AlwaysExpiring existing-effect refresh plan ---

// AuraExistingEffectPlan is the pure disposition for an existing AlwaysExpiring
// aura effect on a member. Host owns SetStartTime / OnEffectLose / SetPower /
// OnEffectGain. Host decides AlwaysExpiring vs add-new path before calling
// PlanAuraExistingAlwaysExpiring (when !hasAlwaysExpiringFlag the host takes
// the add-new path instead).
struct AuraExistingEffectPlan
{
    bool refreshStartTime; // always true on AlwaysExpiring path
    bool updatePower;      // currentPower != newSubPower
};

// PlanAuraExistingAlwaysExpiring returns refresh/power-update flags for the
// existing AlwaysExpiring branch of HandleAura.
// When hasAlwaysExpiringFlag is false, returns an inert plan (host should have
// taken the add-new path instead).
inline auto PlanAuraExistingAlwaysExpiring(
    const bool   hasAlwaysExpiringFlag,
    const uint16 currentPower,
    const uint16 newSubPower) -> AuraExistingEffectPlan
{
    if (!hasAlwaysExpiringFlag)
    {
        return AuraExistingEffectPlan{ false, false };
    }
    return AuraExistingEffectPlan{
        true,
        ShouldUpdateAuraPower(currentPower, newSubPower),
    };
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

// --- Slice 2833: CheckForElevenRoll pure membership set scan ---

// HasElevenRollInSet scans parallel statusID/subPower spans through
// IsElevenRollEffect with production roll ID constants (pure form of
// CStatusEffectContainer::CheckForElevenRoll host scan).
// Scan length is min(statusIDs.size(), subPowers.size()).
// Does not consult deleted (LSB parity with production CheckForElevenRoll).
inline auto HasElevenRollInSet(
    const std::span<const uint16> statusIDs,
    const std::span<const uint16> subPowers) -> bool
{
    const std::size_t n = statusIDs.size() < subPowers.size() ? statusIDs.size() : subPowers.size();
    for (std::size_t i = 0; i < n; ++i)
    {
        if (IsElevenRollEffect(
                statusIDs[i],
                subPowers[i],
                ElevenRollIDFirst,
                ElevenRollIDLast,
                RuneistsRollID))
        {
            return true;
        }
    }
    return false;
}

// PlanCheckForElevenRoll is the plan-named alias for HasElevenRollInSet.
inline auto PlanCheckForElevenRoll(
    const std::span<const uint16> statusIDs,
    const std::span<const uint16> subPowers) -> bool
{
    return HasElevenRollInSet(statusIDs, subPowers);
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


// --- Slice 1368: prevent-action / sleep / confrontation pure lists ---

// Stable status IDs for prevent-action and sleep families.
constexpr uint16 StatusIDSleepI        = 2;
constexpr uint16 StatusIDPetrification = 7;
constexpr uint16 StatusIDStun          = 10;
constexpr uint16 StatusIDCharmI        = 14;
constexpr uint16 StatusIDCharmIi       = 17;
constexpr uint16 StatusIDSleepIi       = 19;
constexpr uint16 StatusIDTerror        = 28;
constexpr uint16 StatusIDPenalty       = 159;
constexpr uint16 StatusIDLullaby       = 193;

// IsPreventActionEffectID (ignoreCharm=false) includes charm IDs.
inline auto IsPreventActionEffectID(const uint16 id, const bool ignoreCharm) -> bool
{
    switch (id)
    {
        case StatusIDSleepI:
        case StatusIDSleepIi:
        case StatusIDPetrification:
        case StatusIDLullaby:
        case StatusIDPenalty:
        case StatusIDStun:
        case StatusIDTerror:
            return true;
        case StatusIDCharmI:
        case StatusIDCharmIi:
            return !ignoreCharm;
        default:
            return false;
    }
}

// IsAsleepEffectID mirrors SleepI/SleepIi/Lullaby set.
inline auto IsAsleepEffectID(const uint16 id) -> bool
{
    return id == StatusIDSleepI || id == StatusIDSleepIi || id == StatusIDLullaby;
}

// IsAsleepStatusID is the slice-2825 name for IsAsleepEffectID (same membership).
inline auto IsAsleepStatusID(const uint16 id) -> bool
{
    return IsAsleepEffectID(id);
}

// IsConfrontationFlag mirrors HasEffectFlag(Confrontation).
inline auto IsConfrontationFlag(const bool hasConfrontationFlag) -> bool
{
    return hasConfrontationFlag;
}

// ConfrontationPowerOrZero returns power when confrontation, else 0.
inline auto ConfrontationPowerOrZero(const bool hasConfrontationFlag, const uint16 power) -> uint16
{
    return hasConfrontationFlag ? power : 0;
}

// ShouldCopyConfrontation mirrors confrontation flag for CopyConfrontationEffect.
inline auto ShouldCopyConfrontation(const bool hasConfrontationFlag) -> bool
{
    return hasConfrontationFlag;
}


// --- Slice 1369: SetEffectParams script path selection pure halves ---

// Source type numeric mirrors (EffectSourceType).
constexpr uint16 SourceTypeNone         = 0;
constexpr uint16 SourceTypeEquippedItem = 1;
constexpr uint16 SourceTypeTemporaryItem = 2;
constexpr uint16 SourceTypeMob          = 3;
constexpr uint16 SourceTypeFood         = 4;

// Selected status IDs for path selection (generated status_effect.h).
constexpr uint16 StatusIDNoneEffect  = 255; // xi::StatusEffect::None
constexpr uint16 StatusIDFood        = 251;
constexpr uint16 StatusIDEnchantment = 162;

// --- Slice 2932: ShouldRejectEffectIDOutOfRange pure dual-wire ---
// Residual pure port: slice 1369 (SetEffectParams path selection suite).
// Production host: CStatusEffectContainer::AddStatusEffect / SetEffectParams
// inject GetStatusID() + MaxEffectID into ShouldRejectEffectIDOutOfRange.
// Go dual-wire: statuseffect.ShouldRejectEffectIDOutOfRange
// (internal/statuseffect/id_range.go).

// MaxEffectID pins MAX_EFFECTID (768 real + 46 custom).
// Dual-wire of Go statuseffect.MaxEffectID (slice 2932; residual 1369).
constexpr uint16 MaxEffectID = 814; // MAX_EFFECTID

// ShouldRejectEffectIDOutOfRange mirrors statusID >= MAX_EFFECTID.
//
// Formula (slice 2932 dual-wire):
//   statusID >= maxEffectID
//
// statusID    — host-evaluated StatusEffect->GetStatusID()
// maxEffectID — production pin MaxEffectID (814 / MAX_EFFECTID)
// true  → reject AddStatusEffect / SetEffectParams (ID out of range)
// false → range gate passes
//
// Dual-wire of Go statuseffect.ShouldRejectEffectIDOutOfRange.
// Call sites: CStatusEffectContainer::AddStatusEffect / SetEffectParams.
inline auto ShouldRejectEffectIDOutOfRange(const uint16 statusID, const uint16 maxEffectID) -> bool
{
    return statusID >= maxEffectID;
}

// ShouldRejectNoneZeroSub mirrors None status with subID == 0.
inline auto ShouldRejectNoneZeroSub(const uint16 statusID, const uint32 subID, const uint16 noneID) -> bool
{
    return statusID == noneID && subID == 0;
}

// HasEffectSource mirrors sourceType != NONE && sourceTypeParam > 0.
inline auto HasEffectSource(const uint16 sourceType, const uint32 sourceTypeParam) -> bool
{
    return sourceType != SourceTypeNone && sourceTypeParam > 0;
}

// IsEquippedItemSource / IsFoodSource.
inline auto IsEquippedItemSource(const uint16 sourceType) -> bool
{
    return sourceType == SourceTypeEquippedItem;
}

inline auto IsFoodSource(const uint16 sourceType) -> bool
{
    return sourceType == SourceTypeFood;
}

// ShouldSetItemScriptName mirrors valid onEffectGain/Lose pair.
inline auto ShouldSetItemScriptName(const bool gainValid, const bool loseValid) -> bool
{
    return gainValid && loseValid;
}

// FormatItemScriptName mirrors "items/" + itemName.
inline auto FormatItemScriptName(const std::string& itemName) -> std::string
{
    return std::string("items/") + itemName;
}

// FormatEffectsScriptName mirrors "effects/" + effectName.
inline auto FormatEffectsScriptName(const std::string& effectName) -> std::string
{
    return std::string("effects/") + effectName;
}

// ShouldUseEffectsScriptPath mirrors the multi-condition gate for effects/ scripts.
inline auto ShouldUseEffectsScriptPath(
    const bool effectFromItemEnchant,
    const bool effectFromItemFood,
    const bool isEnchantmentEffect,
    const bool isEquippedItemSource,
    const bool isFoodEffect,
    const uint32 sourceTypeParam) -> bool
{
    if (effectFromItemEnchant || effectFromItemFood)
    {
        return false;
    }
    if (isEnchantmentEffect)
    {
        return false;
    }
    if (isEquippedItemSource)
    {
        return false;
    }
    // Food with sourceTypeParam > 0 excluded; Food with param 0 allowed.
    if (isFoodEffect && sourceTypeParam != 0)
    {
        return false;
    }
    return true;
}

// ShouldUseItemSubTypeScript mirrors else branch: subType > 0 && item lookup hit.
inline auto ShouldUseItemSubTypeScript(const bool useEffectsPath, const uint32 subType, const bool itemFound) -> bool
{
    return !useEffectsPath && subType > 0 && itemFound;
}


// --- Slice 1371: Add/RemoveStatusEffect pure gates ---

// EffectNotice numeric mirrors.
constexpr uint8 EffectNoticeShowMessage = 0;
constexpr uint8 EffectNoticeSilent      = 1;

// --- Slice 3080 / 3348 / 3428 / 3481 / 3526 / 3562 / 3607 / 3652 / 3697 / 3742 / 3787 / 3832 / 3877: ShouldRejectNullStatusEffect pure dual-wire ---
// Residual pure port: slice 1371 (Add/Remove pure-gate suite).
// Residual dual-wire expand: slice 3080 (test_status_reject_null_3080).
// Prior dedicated dual-wire expand residual: slice 3348 (test_status_reject_null_3348).
// Prior dedicated dual-wire expand residual: slice 3428 (test_status_reject_null_3428).
// Prior dedicated dual-wire expand residual: slice 3481 (test_status_reject_null_3481).
// Prior dedicated dual-wire expand residual: slice 3526 (test_status_reject_null_3526).
// Prior dedicated dual-wire expand residual: slice 3562 (test_status_reject_null_3562).
// Prior dedicated dual-wire expand residual: slice 3607 (test_status_reject_null_3607).
// Prior dedicated dual-wire expand residual: slice 3652 (test_status_reject_null_3652).
// Prior dedicated dual-wire expand residual: slice 3697 (test_status_reject_null_3697).
// Prior dedicated dual-wire expand residual: slice 3742 (test_status_reject_null_3742).
// Prior dedicated dual-wire expand residual: slice 3787 (test_status_reject_null_3787).
// Prior dedicated dual-wire expand residual: slice 3832 (test_status_reject_null_3832).
// Dedicated dual-wire expand residual: slice 3877 (test_status_reject_null_3877).
// Production host: CStatusEffectContainer::AddStatusEffect injects
// (PStatusEffectPtr == nullptr) into ShouldRejectNullStatusEffect; on true
// ShowWarning and return false before ID-range / CanGain.
// Go dual-wire: statuseffect.ShouldRejectNullStatusEffect
// (internal/statuseffect/reject_null_status.go).
// Sibling dual-wires on same Add path: ShouldRejectEffectIDOutOfRange (2932).
// Orthogonal dual-wires left alone: ShouldExpireEffect (3049 / 3225) /
// ShouldTickEffect (3069) / ShouldRejectSimpleImmunity (3113) /
// CanGainWhenNoExisting (3135).
// Index 3080: statuseffect.ShouldRejectNullStatusEffect pure dual-wire (residual).
// Index 3348: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3428: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3481: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3526: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3562: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3607: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3652: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3697: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3742: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3787: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3832: statuseffect.ShouldRejectNullStatusEffect prior dedicated dual-wire
// expand residual 3080 (formula unchanged; retained).
// Index 3877: statuseffect.ShouldRejectNullStatusEffect dedicated dual-wire
// expand residual 3080 (formula unchanged; prior dedicated 3832 / 3787 / 3742 / 3697 / 3652 / 3607 / 3562 / 3526 / 3481 / 3428 / 3348 retained).

// ShouldRejectNullStatusEffect mirrors PStatusEffectPtr == nullptr.
//
// Formula (slice 3877 dedicated dual-wire expand residual 3080; prior dedicated
// 3832 / 3787 / 3742 / 3697 / 3652 / 3607 / 3562 / 3526 / 3481 / 3428 / 3348 / pure 1371 — formula unchanged):
//   isNull
//
// isNull — host-injected (PStatusEffectPtr == nullptr)
// true  → host logs warning and returns false from AddStatusEffect
// false → proceed to ID-range / CanGain / insert path
//
// Dual-wire of Go statuseffect.ShouldRejectNullStatusEffect.
// Call site: CStatusEffectContainer::AddStatusEffect — host injects
// PStatusEffectPtr == nullptr; on true ShowWarning and return false.
// Prior pure port: slice 1371 (add/remove pure-gate suite).
// Residual pins remain in test_status_effect_add_remove_1371; residual dual-wire
// suite is test_status_reject_null_3080. Prior dedicated expand residual suites
// are test_status_reject_null_3348 / test_status_reject_null_3428 /
// test_status_reject_null_3481 / test_status_reject_null_3526 /
// test_status_reject_null_3562 / test_status_reject_null_3607 /
// test_status_reject_null_3652 / test_status_reject_null_3697 /
// test_status_reject_null_3742 / test_status_reject_null_3787 /
// test_status_reject_null_3832. Dedicated expand residual suite is
// test_status_reject_null_3877. Formula is unchanged; this slice only expands
// dual-wire docs + index + dedicated suite.
// Sibling dual-wire: ShouldRejectEffectIDOutOfRange (slice 2932) is next
// on the same AddStatusEffect path. Orthogonal left alone: ShouldExpireEffect
// (3049 / 3225) / ShouldTickEffect (3069) / ShouldRejectSimpleImmunity (3113) /
// CanGainWhenNoExisting (3135).
inline auto ShouldRejectNullStatusEffect(const bool isNull) -> bool
{
    return isNull;
}

// ShouldClampMinDuration mirrors duration < catalog MinDuration.
inline auto ShouldClampMinDuration(const int64 durationUnits, const int64 minDurationUnits) -> bool
{
    return durationUnits < minDurationUnits;
}

// ShouldCheckManeuverAttachments mirrors FireManeuver..DarkManeuver on PC.
inline auto ShouldCheckManeuverAttachments(const uint16 statusID, const bool isPC) -> bool
{
    return isPC && IsManeuverID(statusID);
}

// ShouldUpdateHealthOnGain mirrors health.maxhp != 0 (not mid-login).
inline auto ShouldUpdateHealthOnGain(const bool maxHPNonzero) -> bool
{
    return maxHPNonzero;
}

// ShouldUpdateStatusIconsOnGain mirrors PC && icon != 0.
inline auto ShouldUpdateStatusIconsOnGain(const bool isPC, const uint16 icon) -> bool
{
    return isPC && icon != 0;
}

// ShouldCheckLatentsOnGain mirrors PC && maxHPNonzero.
inline auto ShouldCheckLatentsOnGain(const bool isPC, const bool maxHPNonzero) -> bool
{
    return isPC && maxHPNonzero;
}

// ShouldNotifyLossMessage mirrors notice != Silent && icon != 0 && !NoLossMessage.
inline auto ShouldNotifyLossMessage(
    const bool isSilent,
    const uint16 icon,
    const bool hasNoLossMessageFlag) -> bool
{
    return !isSilent && icon != 0 && !hasNoLossMessageFlag;
}

// ShouldNotifyOriginOnLoss mirrors originId != 0 (and for PC also != self).
inline auto ShouldNotifyOriginOnLoss(const uint32 originID, const uint32 ownerID, const bool isPCOwner) -> bool
{
    if (originID == 0)
    {
        return false;
    }
    if (isPCOwner && originID == ownerID)
    {
        return false;
    }
    return true;
}

// WearOffMessageOrDefault mirrors effectId < MAX ? catalog WearOff : EffectWearsOff(206).
inline auto WearOffMessageOrDefault(
    const uint16 effectID,
    const uint16 maxEffectID,
    const uint16 catalogWearOff,
    const uint16 defaultWearOff) -> uint16
{
    return effectID < maxEffectID ? catalogWearOff : defaultWearOff;
}

// ShouldNotifyNonPCLoss mirrors non-PC path also requires !isDead.
inline auto ShouldNotifyNonPCLoss(const bool shouldNotifyBase, const bool isDead) -> bool
{
    return shouldNotifyBase && !isDead;
}

// ShouldMarkDeleted mirrors !isDeleted before RemoveStatusEffect body.
inline auto ShouldMarkDeleted(const bool alreadyDeleted) -> bool
{
    return !alreadyDeleted;
}

// IsSilentNotice mirrors notice == Silent.
inline auto IsSilentNotice(const uint8 notice, const uint8 silentValue) -> bool
{
    return notice == silentValue;
}


// --- Slice 1372: DelStatusEffectsBy* / KillAll / gain side-effect pure filters ---

// MatchesActiveStatusID mirrors ID match && !deleted.
inline auto MatchesActiveStatusID(const uint16 effectID, const uint16 targetID, const bool deleted) -> bool
{
    return !deleted && effectID == targetID;
}

// MatchesDelBySubID mirrors ID + SubID + !deleted.
inline auto MatchesDelBySubID(
    const uint16 effectID,
    const uint16 targetID,
    const uint32 effectSubID,
    const uint32 targetSubID,
    const bool deleted) -> bool
{
    return !deleted && effectID == targetID && effectSubID == targetSubID;
}

// MatchesDelBySource mirrors ID + sourceType + sourceParam + !deleted.
inline auto MatchesDelBySource(
    const uint16 effectID,
    const uint16 targetID,
    const uint16 effectSourceType,
    const uint16 targetSourceType,
    const uint32 effectSourceParam,
    const uint32 targetSourceParam,
    const bool deleted) -> bool
{
    return !deleted && effectID == targetID && effectSourceType == targetSourceType && effectSourceParam == targetSourceParam;
}

// MatchesDelByTier mirrors ID + tier + !deleted.
inline auto MatchesDelByTier(
    const uint16 effectID,
    const uint16 targetID,
    const uint16 effectTier,
    const uint16 targetTier,
    const bool deleted) -> bool
{
    return !deleted && effectID == targetID && effectTier == targetTier;
}

// ShouldKillTimedEffect mirrors duration != 0 for KillAllStatusEffect.
inline auto ShouldKillTimedEffect(const bool durationNonzero) -> bool
{
    return durationNonzero;
}

// ShouldDespawnPetOnCharm mirrors PC + CharmI/II + hasPet.
inline auto ShouldDespawnPetOnCharm(const bool isPC, const bool isCharmEffect, const bool hasPet) -> bool
{
    return isPC && isCharmEffect && hasPet;
}

// IsCharmStatusID mirrors CharmI or CharmIi.
inline auto IsCharmStatusID(const uint16 statusID) -> bool
{
    return statusID == StatusIDCharmI || statusID == StatusIDCharmIi;
}

// ShouldRewriteSleepIcon mirrors prevent-action active && (SleepIi || Lullaby).
inline auto ShouldRewriteSleepIcon(const bool hasPreventAction, const uint16 statusID) -> bool
{
    return hasPreventAction && (statusID == StatusIDSleepIi || statusID == StatusIDLullaby);
}

// CanClientCancelIcon mirrors icon match && !NoCancel.
inline auto CanClientCancelIcon(const uint16 effectIcon, const uint16 buffNo, const bool hasNoCancelFlag) -> bool
{
    return effectIcon == buffNo && !hasNoCancelFlag;
}

// MatchesEffectType mirrors type equality for DelStatusEffectsByType.
inline auto MatchesEffectType(const uint16 effectType, const uint16 targetType) -> bool
{
    return effectType == targetType && effectType != 0;
}

// MatchesFlagForDelete mirrors HasEffectFlag(flag) for DelStatusEffectsByFlag.
// Optional exclude permanently if duration==0? Production only checks flag.
inline auto MatchesFlagForDelete(const bool hasFlag) -> bool
{
    return hasFlag;
}

// IsCharmEffectForGainSideEffects dual-home of CharmI/II.
inline auto IsCharmEffectForGainSideEffects(const uint16 statusID) -> bool
{
    return IsCharmStatusID(statusID);
}

// ShouldRunGainSideEffects mirrors isAlive owner.
inline auto ShouldRunGainSideEffects(const bool isAlive) -> bool
{
    return isAlive;
}


// Diabolos NM Nightmare sleep tier threshold for DelStatusEffectsByFlag(Damage).
constexpr uint8 DiabolosNightmareSleepTierMin = 5;

// ShouldSkipNightmareSleepOnDamageFlag mirrors Damage flag + SleepI + tier >= 5.
inline auto ShouldSkipNightmareSleepOnDamageFlag(
    const bool flagIncludesDamage,
    const uint16 statusID,
    const uint8 tier) -> bool
{
    return flagIncludesDamage && statusID == StatusIDSleepI && tier >= DiabolosNightmareSleepTierMin;
}

// ShouldRejectZeroEffectType mirrors Type <= 0 for DelStatusEffectsByType.
inline auto ShouldRejectZeroEffectType(const uint16 type) -> bool
{
    return type == 0;
}


// --- Slice 1373: SaveStatusEffects pure persistence filters ---

// Shadow/skin status IDs for power resync before save.
constexpr uint16 StatusIDCopyImage  = 66;
constexpr uint16 StatusIDBlink      = 36;
constexpr uint16 StatusIDStoneskin  = 37;

// ShouldRejectNonPCSave mirrors objtype != TYPE_PC.
inline auto ShouldRejectNonPCSave(const bool isPC) -> bool
{
    return !isPC;
}

// ShouldStripOnSave mirrors (logout && Logout flag) || (!logout && OnZone flag).
inline auto ShouldStripOnSave(const bool logout, const bool hasLogoutFlag, const bool hasOnZoneFlag) -> bool
{
    return (logout && hasLogoutFlag) || (!logout && hasOnZoneFlag);
}

// ShouldSkipDeletedOnSave mirrors isDeleted().
inline auto ShouldSkipDeletedOnSave(const bool deleted) -> bool
{
    return deleted;
}

// ShouldPersistEffect mirrors realDuration > 0 || durationSeconds == 0.
inline auto ShouldPersistEffect(const int64 realDurationSeconds, const int64 durationSeconds) -> bool
{
    return realDurationSeconds > 0 || durationSeconds == 0;
}

// IsCopyImageEffect / IsBlinkEffect / IsStoneskinEffect for power resync.
inline auto IsCopyImageEffect(const uint16 statusID) -> bool
{
    return statusID == StatusIDCopyImage;
}

inline auto IsBlinkEffect(const uint16 statusID) -> bool
{
    return statusID == StatusIDBlink;
}

inline auto IsStoneskinEffect(const uint16 statusID) -> bool
{
    return statusID == StatusIDStoneskin;
}

// ShouldResyncUtsusemiPower mirrors CopyImage before save.
inline auto ShouldResyncUtsusemiPower(const uint16 statusID) -> bool
{
    return IsCopyImageEffect(statusID);
}

// ShouldResyncBlinkPower mirrors Blink before save.
inline auto ShouldResyncBlinkPower(const uint16 statusID) -> bool
{
    return IsBlinkEffect(statusID);
}

// ShouldResyncStoneskinPower mirrors Stoneskin before save.
inline auto ShouldResyncStoneskinPower(const uint16 statusID) -> bool
{
    return IsStoneskinEffect(statusID);
}

// ComputePersistedDurationSeconds:
// if durationSeconds == 0 → 0 (permanent)
// else if OfflineTick → full durationSeconds
// else if realDuration > 0 → realDuration
// else skip (caller uses ShouldPersistEffect first)
inline auto ComputePersistedDurationSeconds(
    const int64 durationSeconds,
    const int64 realDurationSeconds,
    const bool hasOfflineTickFlag) -> uint32
{
    if (durationSeconds <= 0)
    {
        return 0;
    }
    if (hasOfflineTickFlag)
    {
        return static_cast<uint32>(durationSeconds);
    }
    if (realDurationSeconds > 0)
    {
        return static_cast<uint32>(realDurationSeconds);
    }
    return 0;
}

// RealDurationSeconds mirrors count_seconds(start + duration - now).
// Host supplies start+duration and now as absolute seconds (or same unit).
inline auto RealDurationSeconds(const int64 expirySeconds, const int64 nowSeconds) -> int64
{
    return expirySeconds - nowSeconds;
}

// ShouldLoadCopyImageUtsusemi mirrors CopyImage on load path.
inline auto ShouldLoadCopyImageUtsusemi(const uint16 statusID) -> bool
{
    return IsCopyImageEffect(statusID);
}

// ShouldLoadBlinkMod mirrors Blink on load path.
inline auto ShouldLoadBlinkMod(const uint16 statusID) -> bool
{
    return IsBlinkEffect(statusID);
}

// --- Slice 2793: SaveStatusEffects per-effect plan ---

// SaveEffectAction is the pure per-effect disposition of SaveStatusEffects
// before host DelStatusEffect / SQL insert.
enum class SaveEffectAction : uint8
{
    SkipDeleted,   // isDeleted → continue
    Strip,         // Logout/OnZone strip → RemoveStatusEffect + continue
    Persist,       // remaining > 0 or permanent → maybe resync + insert
    DropNoPersist, // not deleted/strip but expired non-permanent → skip insert
};

// SaveStatusEffectPlan is the pure per-effect outcome of SaveStatusEffects.
// Host owns DelStatusEffect (Strip) and SQL (Persist); resync* only set on Persist.
struct SaveStatusEffectPlan
{
    SaveEffectAction action;
    bool             resyncUtsusemi;
    bool             resyncBlink;
    bool             resyncStoneskin;
};

// PlanSaveStatusEffect composes ShouldStripOnSave, ShouldSkipDeletedOnSave,
// ShouldPersistEffect, and ShouldResync* helpers in production loop order:
// 1) strip flags → Strip (runs even for deleted effects that still need strip)
// 2) deleted → SkipDeleted
// 3) persist gate → Persist (+ at most one resync flag by if/else-if)
// 4) else DropNoPersist
inline auto PlanSaveStatusEffect(
    const bool  deleted,
    const bool  logout,
    const bool  hasLogoutFlag,
    const bool  hasOnZoneFlag,
    const int64 realDurationSeconds,
    const int64 durationSeconds,
    const uint16 statusID) -> SaveStatusEffectPlan
{
    if (ShouldStripOnSave(logout, hasLogoutFlag, hasOnZoneFlag))
    {
        return SaveStatusEffectPlan{ SaveEffectAction::Strip, false, false, false };
    }
    if (ShouldSkipDeletedOnSave(deleted))
    {
        return SaveStatusEffectPlan{ SaveEffectAction::SkipDeleted, false, false, false };
    }
    if (ShouldPersistEffect(realDurationSeconds, durationSeconds))
    {
        SaveStatusEffectPlan plan{ SaveEffectAction::Persist, false, false, false };
        if (ShouldResyncUtsusemiPower(statusID))
        {
            plan.resyncUtsusemi = true;
        }
        else if (ShouldResyncBlinkPower(statusID))
        {
            plan.resyncBlink = true;
        }
        else if (ShouldResyncStoneskinPower(statusID))
        {
            plan.resyncStoneskin = true;
        }
        return plan;
    }
    return SaveStatusEffectPlan{ SaveEffectAction::DropNoPersist, false, false, false };
}


// --- Slice 2796: HandleAura ally member eligibility pure plan ---

// ShouldRejectNullAuraMember mirrors PMember == nullptr reject.
inline auto ShouldRejectNullAuraMember(const bool memberNull) -> bool
{
    return memberNull;
}

// IsSameZoneForAura mirrors both zone pointers present and equal zone IDs.
// Host supplies zone IDs (0 when absent) and pointer-present flags.
inline auto IsSameZoneForAura(
    const uint16 ownerZoneID,
    const uint16 memberZoneID,
    const bool   ownerZonePresent,
    const bool   memberZonePresent) -> bool
{
    return ownerZonePresent && memberZonePresent && ownerZoneID == memberZoneID;
}

// IsInAuraRange mirrors distance <= auraRange + modelHitboxSize (slice 2796 name).
// Same semantics as IsWithinAuraRange (slice 1366). Distance is host-injected.
inline auto IsInAuraRange(const float distance, const float auraRange, const float modelHitboxSize) -> bool
{
    return IsWithinAuraRange(distance, auraRange, modelHitboxSize);
}

// ShouldAcceptAuraAlly composes the HandleAura allies member filter:
// !null && sameZone && inRange && !dead.
// Host injects sameZone / inRange / isDead (safe defaults when memberNull).
inline auto ShouldAcceptAuraAlly(
    const bool memberNull,
    const bool sameZone,
    const bool inRange,
    const bool isDead) -> bool
{
    return !ShouldRejectNullAuraMember(memberNull) && sameZone && inRange && !isDead;
}


// --- Slice 2817: HasBustEffect pure membership gate ---

// Bust status effect ID (xi::StatusEffect::Bust / data/status_effects.yaml id 309).
constexpr uint16 StatusIDBust = 309;

// IsBustEffectForAbility mirrors HasBustEffect loop body:
// statusID == Bust && subPower == abilityOrRollID.
// Host injects bustStatusID (typically StatusIDBust / xi::StatusEffect::Bust)
// and the queried ability/roll id. Does not consult deleted (LSB parity).
inline auto IsBustEffectForAbility(
    const uint16 statusID,
    const uint16 subPower,
    const uint16 bustStatusID,
    const uint16 abilityOrRollID) -> bool
{
    return statusID == bustStatusID && subPower == abilityOrRollID;
}

// --- Slice 2820: WakeUp pure ordered status-ID plan ---

// WakeUpStatusIDCount is the number of status IDs WakeUp deletes
// (SleepI, SleepIi, Lullaby).
constexpr std::size_t WakeUpStatusIDCount = 3;

// WakeUpStatusIDs (PlanWakeUp) returns the fixed ordered list of status effect
// IDs that CStatusEffectContainer::WakeUp deletes via DelStatusEffect:
// SleepI, SleepIi, Lullaby. Host injects DelStatusEffect for each id (return
// values ignored, matching production). Reuses StatusIDSleepI / StatusIDSleepIi
// / StatusIDLullaby constants (slice 1368).
inline auto WakeUpStatusIDs() -> std::span<const uint16>
{
    static constexpr uint16 kIDs[] = {
        StatusIDSleepI,
        StatusIDSleepIi,
        StatusIDLullaby,
    };
    return std::span<const uint16>{ kIDs };
}

// PlanWakeUp is the plan-named alias for WakeUpStatusIDs.
inline auto PlanWakeUp() -> std::span<const uint16>
{
    return WakeUpStatusIDs();
}

} // namespace statuseffecthelpers
