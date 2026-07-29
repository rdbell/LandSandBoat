#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>

// Pure CLatentEffectContainer condition evaluation halves extracted so native
// tests can pin ProcessLatentEffect policy without entity graphs.

namespace latenthelpers
{

// ResourcePercent mirrors ((float)current / max) * 100 when max > 0.
// When max is 0 returns 0 (avoids div-by-zero; HP path still runs in production
// even when maxhp is 0 and yields NaN/inf in IEEE — pure path is defensive).
inline auto ResourcePercent(const int32 current, const int32 max) -> float
{
    if (max <= 0)
    {
        return 0.0f;
    }
    return (static_cast<float>(current) / static_cast<float>(max)) * 100.0f;
}

// EvaluateHpUnderPercent mirrors HpUnderPercent: percent <= value.
inline auto EvaluateHpUnderPercent(const int32 hp, const int32 maxhp, const uint16 value) -> bool
{
    return ResourcePercent(hp, maxhp) <= static_cast<float>(value);
}

// EvaluateHpOverPercent mirrors HpOverPercent: percent >= value.
inline auto EvaluateHpOverPercent(const int32 hp, const int32 maxhp, const uint16 value) -> bool
{
    return ResourcePercent(hp, maxhp) >= static_cast<float>(value);
}

// EvaluateHpUnderTpUnder100 mirrors HP% <= value && tp < 1000.
inline auto EvaluateHpUnderTpUnder100(const int32 hp, const int32 maxhp, const int16 tp, const uint16 value) -> bool
{
    return ResourcePercent(hp, maxhp) <= static_cast<float>(value) && tp < 1000;
}

// EvaluateHpOverTpUnder100 mirrors HP% >= value && tp < 1000.
// (Condition name says "TpUnder100" for both HP under/over variants in LSB.)
inline auto EvaluateHpOverTpUnder100(const int32 hp, const int32 maxhp, const int16 tp, const uint16 value) -> bool
{
    return ResourcePercent(hp, maxhp) >= static_cast<float>(value) && tp < 1000;
}

// EvaluateMpUnderPercent mirrors maxmp && percent <= value.
inline auto EvaluateMpUnderPercent(const int32 mp, const int32 maxmp, const uint16 value) -> bool
{
    return maxmp != 0 && ResourcePercent(mp, maxmp) <= static_cast<float>(value);
}

// EvaluateMpUnder mirrors mp <= value.
inline auto EvaluateMpUnder(const int32 mp, const uint16 value) -> bool
{
    return mp <= static_cast<int32>(value);
}

// EvaluateMpOver mirrors MpOver latent: mp >= value (production uses >=).
inline auto EvaluateMpOver(const int32 mp, const uint16 value) -> bool
{
    return mp >= static_cast<int32>(value);
}

// EvaluateMpStrictlyOver mirrors mp > value (WeaponDrawnMpOver path).
inline auto EvaluateMpStrictlyOver(const int32 mp, const uint16 value) -> bool
{
    return mp > static_cast<int32>(value);
}

// EvaluateTpUnder mirrors tp < value.
inline auto EvaluateTpUnder(const int16 tp, const uint16 value) -> bool
{
    return tp < static_cast<int16>(value);
}

// EvaluateTpOver mirrors tp > value.
inline auto EvaluateTpOver(const int16 tp, const uint16 value) -> bool
{
    return tp > static_cast<int16>(value);
}

// EvaluateSubjob mirrors GetSJob() == value.
inline auto EvaluateSubjob(const uint8 sjob, const uint16 value) -> bool
{
    return sjob == static_cast<uint8>(value);
}

// EvaluateMainjob mirrors GetMJob() == value.
inline auto EvaluateMainjob(const uint8 mjob, const uint16 value) -> bool
{
    return mjob == static_cast<uint8>(value);
}

// EvaluateWeaponDrawn mirrors animation == ANIMATION_ATTACK.
inline auto EvaluateWeaponDrawn(const bool isAttackAnimation) -> bool
{
    return isAttackAnimation;
}

// EvaluateWeaponSheathed mirrors animation != ANIMATION_ATTACK.
inline auto EvaluateWeaponSheathed(const bool isAttackAnimation) -> bool
{
    return !isAttackAnimation;
}

// EvaluateJobLevelBelow mirrors main level < value.
inline auto EvaluateJobLevelBelow(const uint8 mlevel, const uint16 value) -> bool
{
    return mlevel < value;
}

// EvaluateJobLevelAbove mirrors main level >= value (LSB uses >= for "above").
// Production: GetMLevel() >= latentEffect.GetConditionsValue() for JobLevelAbove.
inline auto EvaluateJobLevelAbove(const uint8 mlevel, const uint16 value) -> bool
{
    return mlevel >= value;
}

// EvaluateDuringWS mirrors isDuringWs flag.
inline auto EvaluateDuringWS(const bool isDuringWs) -> bool
{
    return isDuringWs;
}

// EvaluatePetID mirrors pet present && pet type && petID match.
inline auto EvaluatePetID(const bool hasPet, const bool isPetType, const uint32 petID, const uint16 value) -> bool
{
    return hasPet && isPetType && petID == value;
}

// EvaluateStatusEffectActive mirrors HasStatusEffect(value).
inline auto EvaluateStatusEffectActive(const bool hasEffect) -> bool
{
    return hasEffect;
}

// EvaluateNoFoodActive mirrors !HasStatusEffect(Food).
inline auto EvaluateNoFoodActive(const bool hasFood) -> bool
{
    return !hasFood;
}

// EvaluateFoodActive mirrors HasStatusEffect(Food) with food item id check host.
// itemMatches is host-evaluated food item id == value (or presence-only inject).
inline auto EvaluateFoodActive(const bool hasFood, const bool itemMatches) -> bool
{
    return hasFood && itemMatches;
}

// EvaluatePartyMembers mirrors value <= partyCount + trustCount.
inline auto EvaluatePartyMembers(const uint16 value, const std::size_t partyCount, const std::size_t trustCount) -> bool
{
    return value <= (partyCount + trustCount);
}

// EvaluatePartyMembersInZone mirrors value <= inZoneCount (members + trusts when leader same zone).
inline auto EvaluatePartyMembersInZone(const uint16 value, const int inZoneCount) -> bool
{
    return value <= static_cast<uint16>(inZoneCount);
}

// EvaluateEquippedInSlot mirrors slot == value.
inline auto EvaluateEquippedInSlot(const uint8 slot, const uint16 value) -> bool
{
    return slot == static_cast<uint8>(value);
}

// EvaluateWeaponDrawnHPUnder mirrors drawn && HP% <= value.
inline auto EvaluateWeaponDrawnHPUnder(const bool isAttackAnimation, const int32 hp, const int32 maxhp, const uint16 value) -> bool
{
    return isAttackAnimation && EvaluateHpUnderPercent(hp, maxhp, value);
}

// EvaluateWeaponDrawnMPOver mirrors drawn && mp > value (strict).
inline auto EvaluateWeaponDrawnMPOver(const bool isAttackAnimation, const int32 mp, const uint16 value) -> bool
{
    return isAttackAnimation && EvaluateMpStrictlyOver(mp, value);
}

// --- Slice 3888: ShouldRejectProcessLatent pure dual-wire ---
// (dedicated expand residual 2961 / pure 1359; prior dedicated 3843 / 3798 / 3753 / 3708 / 3663 / 3618 / 3573 / 3515 / 3467 / 3413 / 3343)
// Residual pure port: slice 1359 (ProcessLatentEffect condition eval suite).
// Residual dual-wire expand: slice 2961 (test_latent_reject_process_2961).
// Prior dedicated dual-wire suite: slice 3343
//   (test_latent_reject_process_3343).
// Prior dedicated dual-wire suite: slice 3413
//   (test_latent_reject_process_3413).
// Prior dedicated dual-wire suite: slice 3467
//   (test_latent_reject_process_3467).
// Prior dedicated dual-wire suite: slice 3515
//   (test_latent_reject_process_3515).
// Prior dedicated dual-wire suite: slice 3573
//   (test_latent_reject_process_3573).
// Prior dedicated dual-wire suite: slice 3618
//   (test_latent_reject_process_3618).
// Prior dedicated dual-wire suite: slice 3663
//   (test_latent_reject_process_3663).
// Prior dedicated dual-wire suite: slice 3708
//   (test_latent_reject_process_3708).
// Prior dedicated dual-wire suite: slice 3753
//   (test_latent_reject_process_3753).
// Prior dedicated dual-wire suite: slice 3798
//   (test_latent_reject_process_3798).
// Prior dedicated dual-wire suite: slice 3843
//   (test_latent_reject_process_3843).
// Dedicated dual-wire suite: slice 3888
//   (test_latent_reject_process_3888).
// Production host: CLatentEffectContainer::ProcessLatentEffect injects
// (m_POwner == nullptr) and (playerZoneID == 0) into ShouldRejectProcessLatent.
// Go dual-wire: latenteffect.ShouldRejectProcessLatent
// (internal/latenteffect/reject_process.go).

// ShouldRejectProcessLatent mirrors owner null or zone id 0.
//
// Formula (slice 3888 dedicated dual-wire; residual expand 2961 / pure 1359 /
// prior dedicated 3343 / 3413 / 3467 / 3515 / 3573 / 3618 / 3663 / 3708 /
// 3753 / 3798 / 3843 — formula unchanged):
//   ownerNull || zoneIsZero
//
// ownerNull  — host-evaluated (m_POwner == nullptr)
// zoneIsZero — host-evaluated (playerZoneID == 0) where
//              playerZoneID = m_POwner != nullptr ? m_POwner->getZone() : 0
// true  → reject ProcessLatentEffect early (return false)
// false → continue condition evaluation
//
// Dual-wire of Go latenteffect.ShouldRejectProcessLatent
// (residual 1359 / residual dual-wire 2961 / prior dedicated 3343 /
// prior dedicated 3413 / prior dedicated 3467 / prior dedicated 3515 /
// prior dedicated 3573 / prior dedicated 3618 / prior dedicated 3663 /
// prior dedicated 3708 / prior dedicated 3753 / prior dedicated 3798 /
// prior dedicated 3843 / dedicated dual-wire 3888).
// Call site: CLatentEffectContainer::ProcessLatentEffect.
// Residual dual-wire suite: 2961 (test_latent_reject_process_2961).
// Prior dedicated dual-wire suite: 3343 (test_latent_reject_process_3343).
// Prior dedicated dual-wire suite: 3413 (test_latent_reject_process_3413).
// Prior dedicated dual-wire suite: 3467 (test_latent_reject_process_3467).
// Prior dedicated dual-wire suite: 3515 (test_latent_reject_process_3515).
// Prior dedicated dual-wire suite: 3573 (test_latent_reject_process_3573).
// Prior dedicated dual-wire suite: 3618 (test_latent_reject_process_3618).
// Prior dedicated dual-wire suite: 3663 (test_latent_reject_process_3663).
// Prior dedicated dual-wire suite: 3708 (test_latent_reject_process_3708).
// Prior dedicated dual-wire suite: 3753 (test_latent_reject_process_3753).
// Prior dedicated dual-wire suite: 3798 (test_latent_reject_process_3798).
// Prior dedicated dual-wire suite: 3843 (test_latent_reject_process_3843).
// Dedicated dual-wire suite: 3888 (test_latent_reject_process_3888).
inline auto ShouldRejectProcessLatent(const bool ownerNull, const bool zoneIsZero) -> bool
{
    return ownerNull || zoneIsZero;
}

// --- Slice 3299: ShouldApplyLatentExpression pure dual-wire ---
// (dedicated expand residual 2983 / pure 1359; prior dedicated expand 3235)
// Residual pure port: slice 1359 (ProcessLatentEffect condition eval suite).
// Residual dual-wire expand: slice 2983 (test_latent_apply_expression_2983).
// Prior dedicated dual-wire suite: slice 3235
//   (test_latenteffect_apply_expression_3235).
// Dedicated dual-wire suite: slice 3299
//   (test_latenteffect_apply_expression_3299).
// Production host: CLatentEffectContainer::ProcessLatentEffect injects
// latentFound (post condition-switch; false only on unhandled default) into
// ShouldApplyLatentExpression before ApplyLatentEffect.
// Go dual-wire: latenteffect.ShouldApplyLatentExpression
// (internal/latenteffect/apply_expression.go).

// ShouldApplyLatentExpression mirrors latentFound before ApplyLatentEffect.
//
// Formula (slice 3299 dedicated dual-wire; residual expand 2983 / pure 1359 —
// formula unchanged; prior dedicated expand 3235):
//   latentFound
//
// latentFound — host-evaluated after the ProcessLatentEffect condition switch:
//               true  when a known latent condition branch set expression
//               false when the switch hit the unhandled-default case
// true  → call ApplyLatentEffect(latentEffect, expression)
// false → skip ApplyLatentEffect and return false
//
// Dual-wire of Go latenteffect.ShouldApplyLatentExpression
// (residual 1359 / residual dual-wire 2983 / prior dedicated 3235 /
//  dedicated dual-wire 3299).
// Call site: CLatentEffectContainer::ProcessLatentEffect (~1287).
// Residual dual-wire suite: 2983 (test_latent_apply_expression_2983).
// Prior dedicated dual-wire suite: 3235 (test_latenteffect_apply_expression_3235).
// Dedicated dual-wire suite: 3299 (test_latenteffect_apply_expression_3299).
inline auto ShouldApplyLatentExpression(const bool latentFound) -> bool
{
    return latentFound;
}

// ApplyLatentWantsActivate mirrors expression true → Activate else Deactivate.
inline auto ApplyLatentWantsActivate(const bool expression) -> bool
{
    return expression;
}

// RegionInWestToAlzadaal mirrors region >= WEST_AHT_URHGAN && <= ALZADAAL.
// region is host-evaluated REGION_TYPE numeric value; bounds are injected as
// bool to keep this header free of zone enums.
inline auto EvaluateSanctionRegionHPUnder(const bool inSanctionRegion, const int32 hp, const int32 maxhp, const uint16 value) -> bool
{
    return inSanctionRegion && ResourcePercent(hp, maxhp) < static_cast<float>(value);
}

inline auto EvaluateSanctionRegionMPUnder(const bool inSanctionRegion, const int32 mp, const int32 maxmp, const uint16 value) -> bool
{
    return inSanctionRegion && maxmp != 0 && ResourcePercent(mp, maxmp) < static_cast<float>(value);
}

inline auto EvaluateSigilRegionHPUnder(const bool inSigilRegion, const int32 hp, const int32 maxhp, const uint16 value) -> bool
{
    return inSigilRegion && ResourcePercent(hp, maxhp) < static_cast<float>(value);
}

inline auto EvaluateSigilRegionMPUnder(const bool inSigilRegion, const int32 mp, const int32 maxmp, const uint16 value) -> bool
{
    return inSigilRegion && maxmp != 0 && ResourcePercent(mp, maxmp) < static_cast<float>(value);
}

// EvaluateSignetBonus mirrors target present, mlevel >= target mlevel, region < WEST_AHT_URHGAN.
inline auto EvaluateSignetBonus(const bool hasTarget, const bool mlevelGteTarget, const bool inConquestRegion) -> bool
{
    return hasTarget && mlevelGteTarget && inConquestRegion;
}

// AvatarAnyID is conditions value 21 meaning any live avatar.
constexpr uint16 AvatarAnyConditionValue = 21;

// EvaluateAvatarMatch mirrors live avatar petID < 21 && (id==value || value==21).
inline auto EvaluateAvatarMatch(const bool isLiveAvatar, const uint32 petID, const uint16 value) -> bool
{
    return isLiveAvatar && petID < 21 && (petID == value || value == AvatarAnyConditionValue);
}

// ProcessLatentListWantsHealthUpdate mirrors any latent changed → UpdateHealth.
inline auto ProcessLatentListWantsHealthUpdate(const bool anyLatentChanged) -> bool
{
    return anyLatentChanged;
}

// AccumulateLatentChange records whether any callback changed a latent while
// allowing the caller to continue processing the remaining effects.
inline auto AccumulateLatentChange(const bool anyLatentChanged, const bool latentChanged) -> bool
{
    return anyLatentChanged || latentChanged;
}

// --- Slice 1360: time / moon / zone / job-multiple / nation / flags ---

// EvaluateZone mirrors getZone() == value.
inline auto EvaluateZone(const uint16 playerZoneID, const uint16 value) -> bool
{
    return playerZoneID == value;
}

// EvaluateSynthTrainee mirrors skill/10 < 40 and no craft imagery.
// skillRankTenths is RealSkills.skill[value] (stored as rank*10).
inline auto EvaluateSynthTrainee(const uint16 skillTenths, const bool anyCraftImagery) -> bool
{
    return (skillTenths / 10) < 40 && !anyCraftImagery;
}

// EvaluateSongRollActive mirrors HasStatusEffectByFlag(Roll|Song).
inline auto EvaluateSongRollActive(const bool hasRollOrSong) -> bool
{
    return hasRollOrSong;
}

// EvaluateElevenRollActive mirrors CheckForElevenRoll().
inline auto EvaluateElevenRollActive(const bool hasElevenRoll) -> bool
{
    return hasElevenRoll;
}

// TimeOfDay param: 0 daytime, 1 nighttime, 2 dusk-dawn.
inline auto EvaluateTimeOfDay(const uint32 vanadielHour, const uint16 value) -> bool
{
    switch (value)
    {
        case 0: // daytime: 06:00 to 18:00
            return vanadielHour >= 6 && vanadielHour < 18;
        case 1: // nighttime: 18:00 to 06:00
            return vanadielHour >= 18 || vanadielHour < 6;
        case 2: // dusk-dawn: 17:00 to 7:00
            return vanadielHour >= 17 || vanadielHour < 7;
        default:
            return false;
    }
}

// HourOfDay param: 1 new day … 6 dead of night (0 unused).
inline auto EvaluateHourOfDay(const uint32 vanadielHour, const uint16 value) -> bool
{
    switch (value)
    {
        case 1: // new day
            return vanadielHour == 4;
        case 2: // dawn
            return vanadielHour >= 6 && vanadielHour < 7;
        case 3: // day
            return vanadielHour >= 7 && vanadielHour < 17;
        case 4: // dusk
            return vanadielHour >= 16 && vanadielHour < 18;
        case 5: // evening
            return vanadielHour >= 18 && vanadielHour < 20;
        case 6: // dead of night
            return vanadielHour >= 20 || vanadielHour < 4;
        default:
            return false;
    }
}

// EvaluateWeekdayMatch mirrors get_weekday == expected weekday constant.
inline auto EvaluateWeekdayMatch(const uint32 weekday, const uint32 expected) -> bool
{
    return weekday == expected;
}

// Moon direction: 1 = waning, 2 = waxing, 0 = neither.
// EvaluateMoonPhase mirrors the 8-way moon phase latent switch.
inline auto EvaluateMoonPhase(const uint32 moonPhase, const uint32 moonDirection, const uint16 value) -> bool
{
    switch (value)
    {
        case 0: // New Moon - 10% waning -> 5% waxing
            return moonPhase <= 5 || (moonPhase <= 10 && moonDirection == 1);
        case 1: // Waxing Crescent - 7% -> 38% waxing
            return moonPhase >= 7 && moonPhase <= 38 && moonDirection == 2;
        case 2: // First Quarter - 40% -> 55% waxing
            return moonPhase >= 40 && moonPhase <= 55 && moonDirection == 2;
        case 3: // Waxing Gibbous - 57% -> 88%
            return moonPhase >= 57 && moonPhase <= 88 && moonDirection == 2;
        case 4: // Full Moon - waxing 90% -> waning 95%
            return moonPhase >= 95 || (moonPhase >= 90 && moonDirection == 2);
        case 5: // Waning Gibbous - 93% -> 62%
            return moonPhase >= 62 && moonPhase <= 93 && moonDirection == 1;
        case 6: // Last Quarter - 60% -> 45%
            return moonPhase >= 45 && moonPhase <= 60 && moonDirection == 1;
        case 7: // Waning Crescent - 43% -> 12%
            return moonPhase >= 12 && moonPhase <= 43 && moonDirection == 1;
        default:
            return false;
    }
}

// EvaluateJobMultiple: value 0 → odd level; else level % value == 0.
inline auto EvaluateJobMultiple(const uint8 mlevel, const uint16 value) -> bool
{
    if (value == 0)
    {
        return mlevel % 2 == 1;
    }
    return mlevel % value == 0;
}

// EvaluateJobMultipleAtNight mirrors JobMultiple && isNight.
inline auto EvaluateJobMultipleAtNight(const uint8 mlevel, const uint16 value, const bool isNight) -> bool
{
    return EvaluateJobMultiple(mlevel, value) && isNight;
}

// EvaluateWeaponDrawnHpAbsolute mirrors hp < value && attack animation
// (WeaponDrawnHpUnder — absolute HP, not percent).
inline auto EvaluateWeaponDrawnHpAbsolute(const bool isAttackAnimation, const int32 hp, const uint16 value) -> bool
{
    return isAttackAnimation && hp < static_cast<int32>(value);
}

// EvaluateWeaponBroken mirrors item non-null && isUnlocked for main/sub/ranged.
inline auto EvaluateWeaponBroken(const bool slotIsWeapon, const bool itemNonNull, const bool isUnlocked) -> bool
{
    return slotIsWeapon && itemNonNull && isUnlocked;
}

// EvaluateInFlag mirrors isInDynamis/Assault/Adoulin style bool host flags.
inline auto EvaluateInFlag(const bool flag) -> bool
{
    return flag;
}

// EvaluateInGarrison mirrors isInGarrison && mlevel >= value.
inline auto EvaluateInGarrison(const bool inGarrison, const uint8 mlevel, const uint16 value) -> bool
{
    return inGarrison && mlevel >= value;
}

// EvaluateNationCitizen mirrors profile.nation == value.
inline auto EvaluateNationCitizen(const uint8 nation, const uint16 value) -> bool
{
    return nation == static_cast<uint8>(value);
}

// EvaluateNationControlUnder mirrors under own nation control (value 0 branch).
inline auto EvaluateNationControlUnder(
    const bool inConquestRegion,
    const bool regionOwnedByPlayerNation,
    const bool hasSignetOrSanctionOrSigil) -> bool
{
    return inConquestRegion && regionOwnedByPlayerNation && hasSignetOrSanctionOrSigil;
}

// EvaluateNationControlOutside mirrors outside own nation (value 1 branch).
inline auto EvaluateNationControlOutside(
    const bool inConquestRegion,
    const bool regionAlwaysOutOfControlOrNotOwned,
    const bool hasSignetOrSanctionOrSigil) -> bool
{
    return inConquestRegion && regionAlwaysOutOfControlOrNotOwned && hasSignetOrSanctionOrSigil;
}

// EvaluateZoneHomeNation mirrors nation match && region == nationRegion.
inline auto EvaluateZoneHomeNation(const uint8 playerNation, const uint8 expectedNation, const bool regionMatches) -> bool
{
    return playerNation == expectedNation && regionMatches;
}

// EvaluateWeatherMatch mirrors weather enum equality.
inline auto EvaluateWeatherMatch(const uint16 weatherValue, const uint16 conditionValue) -> bool
{
    return weatherValue == conditionValue;
}

// EvaluateWeatherElementMatch mirrors weather element equality.
inline auto EvaluateWeatherElementMatch(const uint16 element, const uint16 conditionValue) -> bool
{
    return element == conditionValue;
}

// EvaluateVsTargetValue mirrors target field == value when target present.
inline auto EvaluateVsTargetValue(const bool hasTarget, const uint16 targetValue, const uint16 conditionValue) -> bool
{
    return hasTarget && targetValue == conditionValue;
}

// EvaluateJobInPartyMember mirrors other member (id != owner) has job.
// isOtherMember is member->id != owner id; jobMatches is GetMJob() == value.
inline auto EvaluateJobInPartyMember(const bool isOtherMember, const bool jobMatches) -> bool
{
    return isOtherMember && jobMatches;
}

// IsLiveAvatar mirrors !dead && petID < 21.
inline auto IsLiveAvatar(const bool isDead, const uint32 petID) -> bool
{
    return !isDead && petID < 21;
}

// --- Slice 2835: HasAllLatentsActive pure slot-inactive gate ---

// ShouldMarkNotAllActive mirrors the loop body predicate in
// CLatentEffectContainer::HasAllLatentsActive:
//   !latent.IsActivated() && latent.GetSlot() == slot
// Host injects isActivated and slotMatches (GetSlot() == slot). When true,
// production sets allActive = false (continues scanning; does not early-return).
inline auto ShouldMarkNotAllActive(const bool isActivated, const bool slotMatches) -> bool
{
    return !isActivated && slotMatches;
}

// DoesInactiveLatentDisqualifyAllActive is an alias of ShouldMarkNotAllActive.
inline auto DoesInactiveLatentDisqualifyAllActive(const bool isActivated, const bool slotMatches) -> bool
{
    return ShouldMarkNotAllActive(isActivated, slotMatches);
}

} // namespace latenthelpers
