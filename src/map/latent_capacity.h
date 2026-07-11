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

// ShouldRejectProcessLatent mirrors owner null or zone id 0.
inline auto ShouldRejectProcessLatent(const bool ownerNull, const bool zoneIsZero) -> bool
{
    return ownerNull || zoneIsZero;
}

// ShouldApplyLatentExpression mirrors latentFound before ApplyLatentEffect.
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

} // namespace latenthelpers
