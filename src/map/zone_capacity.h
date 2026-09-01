#pragma once

#include "common/cbasetypes.h"
#include "common/timer.h"

#include "enums/weather.h"
#include "map_constants.h"

#include <chrono>
#include <cstdint>
#include <fmt/format.h>
#include <optional>
#include <set>
#include <string>

// Pure CZone admission / level-restriction / counter policy for native tests.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1363: zone admission / level-restriction / weather / counter policy suite
//   - 2652: ShouldStopZoneTimers (idle shutdown)
//   - 2848: IsMobAwayFromHome / ShouldReportAllMobsHomeAndHealed
//   - 2939: ShouldStampZoneEmptyTime residual dual-wire suite
//           (charListEmpty after DecreaseZoneCounter)
//   - 3347: ShouldStampZoneEmptyTime dedicated dual-wire
//           (stamp_empty.go; expand residual 2939)
//   - 2949: ShouldRejectHighCharTargid residual dual-wire suite
//           (targid >= CharTargidHighThreshold / 0x700)
//   - 3384: ShouldRejectHighCharTargid prior dedicated dual-wire
//           (high_targid.go; expand residual 2949; retained under 3892)
//   - 3457: ShouldRejectHighCharTargid prior dedicated dual-wire
//           (high_targid.go; expand residual 2949; prior dedicated 3384;
//           retained under 3892)
//   - 3520: ShouldRejectHighCharTargid prior dedicated dual-wire
//           (high_targid.go; expand residual 2949; prior dedicated 3457 / 3384;
//           retained under 3892)
//   - 3577: ShouldRejectHighCharTargid prior dedicated dual-wire
//           (high_targid.go; expand residual 2949; prior dedicated 3520 / 3457 /
//           3384; retained under 3892)
//   - 3622: ShouldRejectHighCharTargid prior dedicated dual-wire
//           (high_targid.go; expand residual 2949; prior dedicated 3577 / 3520 /
//           3457 / 3384; retained under 3892)
//   - 3667: ShouldRejectHighCharTargid prior dedicated dual-wire
//           (high_targid.go; expand residual 2949; prior dedicated 3622 / 3577 /
//           3520 / 3457 / 3384; retained under 3892)
//   - 3712: ShouldRejectHighCharTargid prior dedicated dual-wire
//           (high_targid.go; expand residual 2949; prior dedicated 3667 / 3622 /
//           3577 / 3520 / 3457 / 3384; retained under 3892)
//   - 3757: ShouldRejectHighCharTargid prior dedicated dual-wire
//           (high_targid.go; expand residual 2949; prior dedicated 3712 / 3667 /
//           3622 / 3577 / 3520 / 3457 / 3384; retained under 3892)
//   - 3802: ShouldRejectHighCharTargid prior dedicated dual-wire
//           (high_targid.go; expand residual 2949; prior dedicated 3757 / 3712 /
//           3667 / 3622 / 3577 / 3520 / 3457 / 3384; retained under 3892)
//   - 3847: ShouldRejectHighCharTargid prior dedicated dual-wire
//           (high_targid.go; expand residual 2949; prior dedicated 3802 / 3757 /
//           3712 / 3667 / 3622 / 3577 / 3520 / 3457 / 3384; retained under 3892)
//   - 3892: ShouldRejectHighCharTargid dedicated dual-wire
//           (high_targid.go; expand residual 2949; prior dedicated 3847 / 3802 /
//           3757 / 3712 / 3667 / 3622 / 3577 / 3520 / 3457 / 3384)
//   - 2975: ShouldDespawnPCOnLeave (!charListEmpty after DecreaseZoneCounter)
//   - 2992: ShouldCreateZoneTimers (!hasZoneTimerToken && !charListEmpty after InsertPC)
//   - 3019: ShouldRejectInvalidWeather (!isValidEnum / !enum_contains on SetWeather)
//   - 3028: ShouldSkipSameWeather (alreadyCurrent on SetWeather)
//   - 3032: ShouldApplyZoneLevelRestriction residual dual-wire suite
//           (zoneLevelRestriction != 0 on updateCharLevelRestriction)
//   - 3177: ShouldApplyZoneLevelRestriction dedicated dual-wire
//           (apply_level_restriction.go; expand residual 3032)
//   - 3037: ShouldRejectIncreaseZoneCounter residual dual-wire suite
//           (charNull || alreadyInZone || hasTreasurePool on
//           IncreaseZoneCounter entry)
//   - 3224: ShouldRejectIncreaseZoneCounter dedicated dual-wire
//           (reject_increase_counter.go; expand residual 3037)
//   - 3042: ShouldSkipLevelRestrictionUpdate (!hasRestriction → false; else
//           statusNull || powerMatches on updateCharLevelRestriction)
//   - 3043: ShouldDeleteExistingLevelRestriction (hasRestriction && !shouldSkip
//           on updateCharLevelRestriction)
//   - 3048: ShouldClearCostumeOnZoneIn (hasCostume identity on CharZoneIn)
//   - 3053: ShouldForceMorningFog (inFogWindow && selectedBelowHotSpell &&
//           !isCity on UpdateWeather)
//   - 3068: ShouldDismountOnZoneIn (mounted && !canUseMount on CharZoneIn)
//   - 3074: ShouldRescheduleDynamicWeather (!isStatic on UpdateWeather timer wake)
//
// Production host: CZone::DecreaseZoneCounter (zone.cpp) injects
// CharListEmpty() into ShouldStampZoneEmptyTime; on true stamps m_timeZoneEmpty;
// else if ShouldDespawnPCOnLeave calls DespawnPC.
// Go dual-wire: zone.ShouldStampZoneEmptyTime (internal/zone/stamp_empty.go).
// Residual dual-wire suite: 2939 (test_zone_stamp_empty_2939).
// Dedicated dual-wire suite: 3347 (test_zone_stamp_empty_3347).
// Go dual-wire: zone.ShouldDespawnPCOnLeave (internal/zone/despawn_pc_leave.go).
// Production host: CZone::IncreaseZoneCounter (zone.cpp) injects
// PChar == nullptr, loc.zone != nullptr, PTreasurePool != nullptr into
// ShouldRejectIncreaseZoneCounter; on true ShowWarning + return.
// Go dual-wire: zone.ShouldRejectIncreaseZoneCounter
// (internal/zone/reject_increase_counter.go).
// Residual dual-wire suite: 3037 (test_zone_reject_increase_counter_3037).
// Dedicated dual-wire suite: 3224 (test_zone_reject_increase_counter_3224).
// Production host: CZone::IncreaseZoneCounter (zone.cpp) injects
// GetNewCharTargID() into ShouldRejectHighCharTargid; on true ShowError + return.
// Go dual-wire: zone.ShouldRejectHighCharTargid (internal/zone/high_targid.go).
// Residual dual-wire suite: 2949 (test_zone_high_targid_2949).
// Prior dedicated dual-wire suites: 3384 (test_zone_high_targid_3384),
// 3457 (test_zone_high_targid_3457), 3520 (test_zone_high_targid_3520),
// 3577 (test_zone_high_targid_3577), 3622 (test_zone_high_targid_3622),
// 3667 (test_zone_high_targid_3667), 3712 (test_zone_high_targid_3712),
// 3757 (test_zone_high_targid_3757), 3802 (test_zone_high_targid_3802),
// 3847 (test_zone_high_targid_3847).
// Dedicated dual-wire suite: 3892 (test_zone_high_targid_3892).
// Production host: CZone::IncreaseZoneCounter (zone.cpp) injects
// zoneTimerToken_.has_value() and CharListEmpty() into ShouldCreateZoneTimers
// after InsertPC; on true calls createZoneTimers().
// Go dual-wire: zone.ShouldCreateZoneTimers (internal/zone/create_zone_timers.go).
// Production host: CZone::SetWeather (zone.cpp) injects
// magic_enum::enum_contains<Weather>(weather) into ShouldRejectInvalidWeather;
// on true ShowWarningFmt(FormatInvalidWeatherWarning) + return.
// Go dual-wire: zone.ShouldRejectInvalidWeather
// (internal/zone/reject_invalid_weather.go).
// Production host: CZone::SetWeather (zone.cpp) injects
// weather_.current() == weather into ShouldSkipSameWeather; on true return.
// Go dual-wire: zone.ShouldSkipSameWeather (internal/zone/skip_same_weather.go).
// Production host: CZone::updateCharLevelRestriction (zone.cpp) injects
// hasRestriction, statusEffect==nullptr, and power==m_levelRestriction into
// ShouldSkipLevelRestrictionUpdate; on true return early.
// Else injects hasRestriction / shouldSkip into
// ShouldDeleteExistingLevelRestriction; on true DelStatusEffect LevelRestriction.
// Then injects m_levelRestriction into ShouldApplyZoneLevelRestriction; on true
// strips buffs + AddStatusEffect LevelRestriction.
// Go dual-wire: zone.ShouldSkipLevelRestrictionUpdate
// (internal/zone/skip_level_restriction.go);
// zone.ShouldDeleteExistingLevelRestriction
// (internal/zone/delete_level_restriction.go);
// zone.ShouldApplyZoneLevelRestriction
// (internal/zone/apply_level_restriction.go).
// Production host: CZone::CharZoneIn (zone.cpp) injects
// isMounted() and CanUseMisc(MISC_MOUNT) into ShouldDismountOnZoneIn; on true
// animation = NONE + DelStatusEffectSilent(Mounted).
// Go dual-wire: zone.ShouldDismountOnZoneIn (internal/zone/mount_gate.go).
// Production host: CZone::CharZoneIn (zone.cpp) injects
// HasStatusEffect(Costume) into ShouldClearCostumeOnZoneIn; on true
// DelStatusEffectSilent(Costume). Sibling ShouldDismountOnZoneIn (3068) runs
// just before this clear.
// Go dual-wire: zone.ShouldClearCostumeOnZoneIn (internal/zone/costume_gate.go).
// Production host: CZone::UpdateWeather (zone.cpp) injects
// (CurrentVanaDate in [StartFog, EndFog)), selectedWeather < Weather::HotSpell,
// and (GetTypeMask() & ZONE_TYPE::CITY) != 0 into ShouldForceMorningFog; on true
// forces selectedWeather = Fog and clamps WeatherNextUpdate to EndFogVanaDate.
// Go dual-wire: zone.ShouldForceMorningFog (internal/zone/morning_fog.go).
// Production host: CZone::UpdateWeather (zone.cpp) posts a main-thread task that
// yields for WeatherNextUpdate, then injects weather().isStatic() into
// ShouldRescheduleDynamicWeather; on true calls UpdateWeather() again.
// Go dual-wire: zone.ShouldRescheduleDynamicWeather
// (internal/zone/reschedule_weather.go). Sibling weather dual-wire:
// ShouldForceMorningFog (3053) runs earlier in UpdateWeather before SetWeather.

namespace zonehelpers
{

// IsResidentialArea mirrors zoneutils::IsResidentialArea after the host reads
// CCharEntity::inMogHouse().
inline auto IsResidentialArea(const bool inMogHouse) -> bool
{
    return inMogHouse;
}

// CharTargidHighThreshold is the exclusive upper bound for player targids
// (0x700). At or above this value, insert packets are ignored.
constexpr uint16 CharTargidHighThreshold = 0x700;

struct CharTargidAllocation
{
    uint16 targid;
    bool   high;
};

// AllocateCharTargid mirrors CZoneEntities::GetNewCharTargID. The supplied
// set is ordered like m_charTargIds; allocation starts at 0x400 and stops at
// the first value that does not equal the current candidate.
inline auto AllocateCharTargid(const std::set<uint16>& usedTargids) -> CharTargidAllocation
{
    uint16 targid = 0x400;
    for (const auto used : usedTargids)
    {
        if (targid != used)
        {
            break;
        }
        ++targid;
    }
    return { targid, targid >= CharTargidHighThreshold };
}

// FormatIncreaseZoneCounterWarning mirrors IncreaseZoneCounter ShowWarning text.
inline auto FormatIncreaseZoneCounterWarning() -> std::string
{
    return "CZone::IncreaseZoneCounter() - PChar is null, or Player zone or Treasure Pools is not null.";
}

// FormatInsertCharTargidHighError mirrors ShowError when targid >= 0x700.
// Note production uses a printf-style format with a broken specifier ("03hX");
// the pure helper pins the fixed prefix only.
inline auto FormatInsertCharTargidHighErrorPrefix() -> std::string
{
    return "CZone::InsertChar : targid is high (03hX), update packets will be ignored";
}

// ---------------------------------------------------------------------------
// Slice 3224 — IncreaseZoneCounter entry reject (dedicated expand residual 3037)
// ---------------------------------------------------------------------------

// ShouldRejectIncreaseZoneCounter mirrors
// PChar null || loc.zone != null || PTreasurePool != null.
//
// Formula (slice 3224 dedicated dual-wire; residual expand 3037 / pure 1363 —
// formula unchanged):
//   charNull || alreadyInZone || hasTreasurePool
//
// charNull         — host-evaluated PChar == nullptr
// alreadyInZone    — host-evaluated PChar != nullptr && PChar->loc.zone != nullptr
// hasTreasurePool  — host-evaluated PChar != nullptr && PChar->PTreasurePool != nullptr
// true  → ShowWarning + return (no GetNewCharTargID / InsertPC)
// false → admit enter path (targid assign may proceed)
//
// Dense 2³ space: only (false, false, false) is false (accept); any true pole
// rejects. Production host short-circuits null injects so alreadyInZone /
// hasTreasurePool are false when charNull is true, but the pure helper still
// evaluates the three injected bools independently.
//
// Dual-wire of Go zone.ShouldRejectIncreaseZoneCounter.
// Call site: CZone::IncreaseZoneCounter entry — host injects null / loc.zone /
// PTreasurePool presence flags.
// Prior pure port: slice 1363 (zone policy suite). Residual dual-wire suite:
// 3037 / test_zone_reject_increase_counter_3037. Dedicated dual-wire suite is
// test_zone_reject_increase_counter_3224. Residual pins remain in
// test_zone_policy_1363.
// Sibling enter gates: ShouldRejectHighCharTargid (2949), ShouldCreateZoneTimers (2992).
inline auto ShouldRejectIncreaseZoneCounter(
    const bool charNull,
    const bool alreadyInZone,
    const bool hasTreasurePool) -> bool
{
    return charNull || alreadyInZone || hasTreasurePool;
}

// ShouldRejectHighCharTargid mirrors targid >= 0x700 after GetNewCharTargID.
//
// Formula (slice 3892 dedicated dual-wire expand residual 2949; prior dedicated
// 3847 / 3802 / 3757 / 3712 / 3667 / 3622 / 3577 / 3520 / 3457 / 3384; pure 1363 — formula unchanged):
//   targid >= CharTargidHighThreshold
//
// CharTargidHighThreshold is pinned at 0x700 (same as Go zone.CharTargidHighThreshold).
// true  → reject insert (ShowError high-targid prefix; update packets ignored)
// false → accept insert path (InsertPC may proceed)
//
// Dual-wire of Go zone.ShouldRejectHighCharTargid.
// Call site: CZone::IncreaseZoneCounter after GetNewCharTargID assigns PChar->targid.
// Prior pure port: slice 1363 (zone policy suite). Residual dual-wire suite:
// 2949 / test_zone_high_targid_2949. Prior dedicated dual-wire suites are
// test_zone_high_targid_3384, test_zone_high_targid_3457,
// test_zone_high_targid_3520, test_zone_high_targid_3577,
// test_zone_high_targid_3622, test_zone_high_targid_3667,
// test_zone_high_targid_3712, test_zone_high_targid_3757,
// test_zone_high_targid_3802, and test_zone_high_targid_3847.
// Dedicated dual-wire suite is test_zone_high_targid_3892. Residual pins remain
// in test_zone_policy_1363.
// Sibling enter gates: ShouldRejectIncreaseZoneCounter (3224),
// ShouldCreateZoneTimers (2992).
inline auto ShouldRejectHighCharTargid(const uint16 targid) -> bool
{
    return targid >= CharTargidHighThreshold;
}

// ShouldCreateZoneTimers mirrors !zoneTimerToken && !CharListEmpty after insert.
//
// Formula (slice 2992 dual-wire):
//   !hasZoneTimerToken && !charListEmpty
//
// hasZoneTimerToken — host-evaluated zoneTimerToken_.has_value() after InsertPC
// charListEmpty     — host-evaluated m_zoneEntities->CharListEmpty() after InsertPC
// true  → createZoneTimers() (first PC joined with no active zone timer)
// false → timers already running, or char list still empty
//
// Dense 2² space: only (token=false, empty=false) is true; all other poles false.
//
// Dual-wire of Go zone.ShouldCreateZoneTimers.
// Call site: CZone::IncreaseZoneCounter after m_zoneEntities->InsertPC —
// host injects zoneTimerToken_.has_value() and CharListEmpty().
// Prior pure port: slice 1363 (zone policy suite). Residual pins remain in
// test_zone_policy_1363; dedicated dual-wire suite is test_zone_create_timers_2992.
// Sibling leave gates: ShouldStampZoneEmptyTime (3347 / residual 2939),
// ShouldDespawnPCOnLeave (2975).
inline auto ShouldCreateZoneTimers(const bool hasZoneTimerToken, const bool charListEmpty) -> bool
{
    return !hasZoneTimerToken && !charListEmpty;
}

// ---------------------------------------------------------------------------
// Slice 3347 — DecreaseZoneCounter empty-stamp (dedicated expand residual 2939)
// ---------------------------------------------------------------------------

// ShouldStampZoneEmptyTime mirrors CharListEmpty after DecreaseZoneCounter.
//
// Formula (slice 3347 dedicated dual-wire; residual expand 2939 / pure 1363 —
// formula unchanged):
//   charListEmpty
//
// charListEmpty — host-evaluated m_zoneEntities->CharListEmpty() after decrease
// true  → stamp m_timeZoneEmpty = timer::now() (zone idle-empty clock starts)
// false → else-branch: ShouldDespawnPCOnLeave may DespawnPC for remaining PCs
//
// Dense 2¹ space: stamp iff charListEmpty is true; false leaves the despawn
// else-branch to ShouldDespawnPCOnLeave.
//
// Dual-wire of Go zone.ShouldStampZoneEmptyTime.
// Call site: CZone::DecreaseZoneCounter after DecreaseZoneCounter entities update.
// Prior pure port: slice 1363 (zone policy suite). Residual dual-wire suite:
// 2939 / test_zone_stamp_empty_2939. Dedicated dual-wire suite is
// test_zone_stamp_empty_3347. Residual pins remain in test_zone_policy_1363.
// Sibling leave gate: ShouldDespawnPCOnLeave (2975).
inline auto ShouldStampZoneEmptyTime(const bool charListEmpty) -> bool
{
    return charListEmpty;
}

// ShouldDespawnPCOnLeave mirrors !CharListEmpty after DecreaseZoneCounter
// (else-branch of the empty-stamp gate).
//
// Formula (slice 2975 dual-wire):
//   !charListEmpty
//
// charListEmpty — host-evaluated m_zoneEntities->CharListEmpty() after decrease
// true  → DespawnPC(PChar) for the leaving char (other PCs remain in zone)
// false → no despawn here; ShouldStampZoneEmptyTime stamps m_timeZoneEmpty
//
// Polarity note: opposite of ShouldStampZoneEmptyTime (true when NOT empty).
//
// Dual-wire of Go zone.ShouldDespawnPCOnLeave.
// Call site: CZone::DecreaseZoneCounter after ShouldStampZoneEmptyTime —
// host injects CharListEmpty(); else-branch calls DespawnPC(PChar).
// Prior pure port: slices 1363 / 2706 residual (zone policy suite). Residual
// pins remain in test_zone_policy_1363; dedicated dual-wire suite is
// test_zone_despawn_pc_leave_2975. Sibling: ShouldStampZoneEmptyTime
// (3347 dedicated / residual 2939).
inline auto ShouldDespawnPCOnLeave(const bool charListEmpty) -> bool
{
    return !charListEmpty;
}

// ShouldStopZoneTimers mirrors CZone::ZoneServer's idle shutdown gate. The
// five-second delay is strict: exactly five seconds after the zone became
// empty is still retained.
inline auto ShouldStopZoneTimers(
    const bool              hasZoneTimer,
    const bool              charListEmpty,
    const timer::time_point zoneEmptyTime,
    const timer::time_point now,
    const bool              allMobsHomeAndHealed) -> bool
{
    return hasZoneTimer && charListEmpty && zoneEmptyTime + std::chrono::seconds(5) < now && allMobsHomeAndHealed;
}

struct ZoneTimerPlan
{
    bool                      install{};
    std::chrono::milliseconds logicInterval{};
    std::chrono::milliseconds triggerAreaInterval{};
};

// PlanZoneTimers mirrors CZone::createZoneTimers. Test servers manually
// advance ticks and therefore install neither recurring timer.
inline auto PlanZoneTimers(const bool isTestServer) -> ZoneTimerPlan
{
    if (isTestServer)
    {
        return {};
    }
    return { true, kLogicUpdateInterval, kTriggerAreaInterval };
}

// PlanSealTimerExpiry mirrors CharZoneOut's optional SealTimerExpiry save.
inline auto PlanSealTimerExpiry(const bool persistEnabled, const bool hasRecast, const timer::duration remaining, const uint32 now) -> std::optional<uint32>
{
    if (!persistEnabled || !hasRecast || remaining <= std::chrono::seconds(10))
    {
        return std::nullopt;
    }
    return now + static_cast<uint32>(std::chrono::duration_cast<std::chrono::seconds>(remaining).count());
}

struct SealTimerRestorePlan { bool restore{}; uint32 remainingSeconds{}; bool clearStoredExpiry{}; };
inline auto PlanSealTimerRestore(const bool persistEnabled, const uint32 expiry, const uint32 now) -> SealTimerRestorePlan
{
    if (!persistEnabled || expiry == 0) return {};
    const auto remaining = expiry > now ? expiry - now : 0;
    return { remaining > 0 && remaining <= 300, remaining, true };
}

template <typename Areas, typename IsMember>
auto FirstZoneOutTriggerArea(const Areas& areas, IsMember&& isMember) -> typename Areas::const_iterator
{
    for (auto it = areas.begin(); it != areas.end(); ++it) if (isMember((*it)->getTriggerAreaID())) return it;
    return areas.end();
}

// --- updateCharLevelRestriction ---

// ShouldSkipLevelRestrictionUpdate mirrors already has LevelRestriction with
// same power as zone restriction (or null status effect after Has check quirk)
// on CZone::updateCharLevelRestriction admission.
//
// Formula (slice 3042 dual-wire):
//
//   if (!hasRestriction) return false;
//   return statusNull || powerMatches;
//
// hasRestriction — host-evaluated HasStatusEffect(LevelRestriction)
// statusNull — GetStatusEffect returned nullptr after Has said true (defensive)
// powerMatches — statusEffect->GetPower() == m_levelRestriction
// true  → return early (existing restriction already matches zone / null quirk)
// false → continue (may ShouldDeleteExistingLevelRestriction + apply)
//
// Dual-wire of Go zone.ShouldSkipLevelRestrictionUpdate.
// Call site: CZone::updateCharLevelRestriction — when hasRestriction; host
// injects statusNull and powerMatches before optional delete/apply.
// Residual 1363 pins remain in test_zone_policy_1363; dedicated dual-wire
// suite is test_zone_skip_level_restriction_3042.
// Sibling level-restriction gates: ShouldDeleteExistingLevelRestriction
// (3043 dual-wire), ShouldApplyZoneLevelRestriction (3177 dual-wire;
// residual 3032).
inline auto ShouldSkipLevelRestrictionUpdate(
    const bool hasRestriction,
    const bool statusNull,
    const bool powerMatches) -> bool
{
    if (!hasRestriction)
    {
        return false;
    }
    return statusNull || powerMatches;
}

// ShouldDeleteExistingLevelRestriction mirrors has restriction and not skip
// on CZone::updateCharLevelRestriction (delete existing LevelRestriction when
// present and the skip gate did not fire).
//
// Formula (slice 3043 dual-wire):
//
//   hasRestriction && !shouldSkip
//
// hasRestriction — host-evaluated HasStatusEffect(LevelRestriction)
// shouldSkip     — host-evaluated ShouldSkipLevelRestrictionUpdate result
// true  → DelStatusEffect(LevelRestriction) before optional apply
// false → no delete (no existing restriction, or skip already decided)
//
// Dense 2² space: only (has=true, skip=false) is true (delete); all other
// poles false. Production call site after the skip early-return injects
// (true, false) because hasRestriction is already true in that branch and
// shouldSkip is known false (skip would have returned). The pure helper still
// evaluates both injected bools independently.
//
// Dual-wire of Go zone.ShouldDeleteExistingLevelRestriction.
// Call site: CZone::updateCharLevelRestriction — after
// ShouldSkipLevelRestrictionUpdate (sibling 3042); host then optionally
// applies zone cap via ShouldApplyZoneLevelRestriction (3177 / residual 3032).
// Residual 1363 pins remain in test_zone_policy_1363; dedicated dual-wire
// suite is test_zone_delete_level_restriction_3043.
// Sibling level-restriction gates: ShouldSkipLevelRestrictionUpdate (3042),
// ShouldApplyZoneLevelRestriction (3177 dual-wire; residual 3032).
inline auto ShouldDeleteExistingLevelRestriction(
    const bool hasRestriction,
    const bool shouldSkip) -> bool
{
    return hasRestriction && !shouldSkip;
}

// ShouldApplyZoneLevelRestriction mirrors m_levelRestriction != 0 on
// CZone::updateCharLevelRestriction admission (apply LevelRestriction when
// the zone has a non-zero cap).
//
// Formula (slice 3177 dedicated dual-wire; residual expand 3032 / pure 1363 —
// formula unchanged):
//
//   zoneLevelRestriction != 0
//
// zoneLevelRestriction — host-injected m_levelRestriction (uint8)
// true  → DelStatusEffectsByFlag (dispelable/erasable/…) + AddStatusEffect LevelRestriction
// false → no apply (zone has no level cap)
//
// Dual-wire of Go zone.ShouldApplyZoneLevelRestriction
// (residual 1363 / residual dual-wire 3032 / dedicated dual-wire 3177).
// Call site: CZone::updateCharLevelRestriction — after optional skip/delete
// of an existing LevelRestriction; host injects m_levelRestriction.
// Residual 1363 pins remain in test_zone_policy_1363.
// Residual dual-wire suite: 3032 (test_zone_apply_level_restriction_3032).
// Dedicated dual-wire suite: 3177 (test_zone_apply_level_restriction_3177).
// Sibling level-restriction gates: ShouldSkipLevelRestrictionUpdate
// (3042 dual-wire), ShouldDeleteExistingLevelRestriction (3043 dual-wire).
inline auto ShouldApplyZoneLevelRestriction(const uint8 zoneLevelRestriction) -> bool
{
    return zoneLevelRestriction != 0;
}

// --- SetWeather ---

// FormatInvalidWeatherWarning mirrors ShowWarningFmt for invalid weather enum.
inline auto FormatInvalidWeatherWarning(const uint16 weatherValue) -> std::string
{
    return fmt::format("Weather value ({}) invalid.", weatherValue);
}

// ShouldRejectInvalidWeather mirrors !magic_enum::enum_contains<Weather>(weather)
// on CZone::SetWeather admission.
//
// Formula (slice 3019 dual-wire):
//
//   !isValidEnum
//
// isValidEnum — host-evaluated magic_enum::enum_contains<Weather>(weather)
// true  → ShowWarningFmt(FormatInvalidWeatherWarning) + return (reject set)
// false → continue (may still ShouldSkipSameWeather / WeatherChange)
//
// Dual-wire of Go zone.ShouldRejectInvalidWeather.
// Call site: CZone::SetWeather — host injects enum_contains result.
// Residual 1363 pins remain in test_zone_policy_1363; dedicated dual-wire
// suite is test_zone_reject_invalid_weather_3019.
// Sibling SetWeather gates: ShouldSkipSameWeather (3028 dual-wire),
// FormatInvalidWeatherWarning (residual 1363).
inline auto ShouldRejectInvalidWeather(const bool isValidEnum) -> bool
{
    return !isValidEnum;
}

// ShouldSkipSameWeather mirrors weather_.current() == weather on
// CZone::SetWeather admission (already-current identity skip).
//
// Formula (slice 3028 dual-wire):
//
//   alreadyCurrent
//
// alreadyCurrent — host-evaluated weather_.current() == weather
// true  → return early (no WeatherChange / set / packet)
// false → continue (WeatherChange + weather_.set + WEATHER packet)
//
// Dual-wire of Go zone.ShouldSkipSameWeather.
// Call site: CZone::SetWeather — after ShouldRejectInvalidWeather; host
// injects current==requested comparison.
// Residual 1363 pins remain in test_zone_policy_1363; dedicated dual-wire
// suite is test_zone_skip_same_weather_3028.
// Sibling SetWeather gates: ShouldRejectInvalidWeather (3019 dual-wire),
// FormatInvalidWeatherWarning (residual 1363).
inline auto ShouldSkipSameWeather(const bool alreadyCurrent) -> bool
{
    return alreadyCurrent;
}

// ShouldDisableScentForWeather mirrors the weather membership test in
// CZoneEntities::WeatherChange. The caller applies this only to mobs whose
// MOBMOD_DETECTION contains DETECT_SCENT.
//
// Rain, squall, and blizzards disable scent detection; every other weather
// leaves it enabled.
inline auto ShouldDisableScentForWeather(const Weather weather) -> bool
{
    return weather == Weather::Rain || weather == Weather::Squall || weather == Weather::Blizzards;
}

// ShouldSuppressHiddenGMEntityUpdate mirrors CZoneEntities::UpdateEntityPacket's
// early return for hidden player characters. Despawns must still be sent so
// recipients remove an entity that was previously visible.
inline auto ShouldSuppressHiddenGMEntityUpdate(const bool hiddenGM, const bool isDespawn) -> bool
{
    return hiddenGM && !isDespawn;
}

// ShouldUseGridEntityUpdateRouting selects UpdateEntityPacket's spatial-grid
// fast path. Only ordinary updates without the always-include override use the
// grid, and only when it contains tracked entities.
inline auto ShouldUseGridEntityUpdateRouting(const bool isEntityUpdate, const bool alwaysInclude, const bool gridNonEmpty) -> bool
{
    return isEntityUpdate && !alwaysInclude && gridNonEmpty;
}

// ShouldDispatchEntityUpdateToRecipient mirrors UpdateEntityPacket's fallback
// recipient admission. Spawns, despawns, and always-include updates bypass an
// existing spawn-list membership check.
inline auto ShouldDispatchEntityUpdateToRecipient(
    const bool alwaysInclude,
    const bool isSpawn,
    const bool isDespawn,
    const bool entityAlreadySpawned) -> bool
{
    return alwaysInclude || isSpawn || isDespawn || entityAlreadySpawned;
}

// ShouldSpawnNearbyNPC mirrors TryAddToNearbySpawnLists' NPC path. NPCs bypass
// the vertical render limit so elevators remain visible, but still require the
// normal horizontal render range.
inline auto ShouldSpawnNearbyNPC(const bool inRange) -> bool
{
    return inRange;
}

// ShouldSpawnNearbyNonNPC mirrors TryAddToNearbySpawnLists' shared admission
// for players, mobs, pets, and trusts.
inline auto ShouldSpawnNearbyNonNPC(const bool inRange, const bool withinVerticalRange) -> bool
{
    return inRange && withinVerticalRange;
}

// ShouldSpawnNearbyPC mirrors the player-specific privacy checks after the
// shared range gates. Players in different Mog Houses and hidden GMs are not
// added to nearby player spawn lists.
inline auto ShouldSpawnNearbyPC(const bool sameMogHouse, const bool hiddenGM) -> bool
{
    return sameMogHouse && !hiddenGM;
}

// WeatherPacketOffsetMin/Max for xirand::GetRandomNumber(4, 28) half-open.
constexpr uint16 WeatherPacketOffsetMin = 4;
constexpr uint16 WeatherPacketOffsetMaxExclusive = 28;

// --- CheckMobsPathedBack ---

// IsMobAwayFromHome mirrors (alive && !fullyHealed) || pathingHome.
// isDead false means alive/spawned for this check.
inline auto IsMobAwayFromHome(const bool isDead, const bool isFullyHealed, const bool isPathingHome) -> bool
{
    return ((!isDead && !isFullyHealed) || isPathingHome);
}

// ShouldReportAllMobsHomeAndHealed mirrors empty list or no away mobs.
// anyAway is true if any mob matched IsMobAwayFromHome.
inline auto ShouldReportAllMobsHomeAndHealed(const bool hasMobList, const bool anyAway) -> bool
{
    if (!hasMobList)
    {
        return true;
    }
    return !anyAway;
}

// CanUseMisc mirrors (miscMask & misc) == misc.
inline auto CanUseMisc(const uint16 miscMask, const uint16 misc) -> bool
{
    return (miscMask & misc) == misc;
}

// ShouldDismountOnZoneIn mirrors isMounted && !CanUseMisc(MISC_MOUNT) on CharZoneIn.
//
// Formula (slice 3068 dual-wire):
//   mounted && !canUseMount
//
// mounted     — host-evaluated PChar->isMounted()
// canUseMount — host-evaluated CanUseMisc(MISC_MOUNT) for the destination zone
// true  → force dismount (animation NONE + DelStatusEffectSilent Mounted)
// false → keep mount state (not mounted, or zone allows mounts)
//
// Dense 2² space: only (mounted=true, canUseMount=false) is true; all other poles false.
//
// Dual-wire of Go zone.ShouldDismountOnZoneIn.
// Call site: CZone::CharZoneIn — host injects isMounted() and CanUseMisc(MISC_MOUNT).
// Prior pure port: slice 2673 (zone-in mount dismount gate). Residual pins remain in
// test_zone_mount_gate_2673; dedicated dual-wire suite is
// test_zone_dismount_zone_in_3068. Sibling zone-in gate: ShouldClearCostumeOnZoneIn
// (3048) runs just after this dismount.
inline auto ShouldDismountOnZoneIn(const bool mounted, const bool canUseMount) -> bool
{
    return mounted && !canUseMount;
}

// ShouldClearCostumeOnZoneIn mirrors HasStatusEffect(Costume) on CharZoneIn.
//
// Formula (slice 3048 dual-wire):
//   hasCostume
//
// hasCostume — host-evaluated StatusEffectContainer->HasStatusEffect(Costume)
// true  → DelStatusEffectSilent(Costume) during zone-in
// false → no costume clear (effect absent)
//
// Identity gate: clear costume status on zone-in when present.
//
// Dual-wire of Go zone.ShouldClearCostumeOnZoneIn.
// Call site: CZone::CharZoneIn — host injects HasStatusEffect(Costume).
// Prior pure port: slice 2682 (zone-in Costume gate). Residual pins remain in
// test_zone_costume_gate_2682; dedicated dual-wire suite is
// test_zone_clear_costume_3048. Sibling zone-in gate: ShouldDismountOnZoneIn
// (3068 dual-wire; residual 2673) runs just before this clear.
inline auto ShouldClearCostumeOnZoneIn(const bool hasCostume) -> bool
{
    return hasCostume;
}

// WeatherCycleDays mirrors zone WeatherCycle constant used in UpdateWeather.
constexpr uint32 WeatherCycleDays = 2160;

// Weather chance bands from UpdateWeather.
constexpr uint8 WeatherChanceRareExclusive   = 15; // [0, 15)
constexpr uint8 WeatherChanceCommonExclusive = 50; // [15, 50)
// [50, 100) normal

// SelectWeatherBand: 0 rare, 1 common, 2 normal.
inline auto SelectWeatherBand(const uint8 weatherChance) -> uint8
{
    if (weatherChance < WeatherChanceRareExclusive)
    {
        return 0;
    }
    if (weatherChance < WeatherChanceCommonExclusive)
    {
        return 1;
    }
    return 2;
}

// ShouldForceMorningFog mirrors fog window + non-elemental + non-city
// on CZone::UpdateWeather morning fog override.
//
// Formula (slice 3053 dual-wire):
//
//   inFogWindow && selectedBelowHotSpell && !isCity
//
// inFogWindow — host-evaluated Vana'diel time in [02:00, 07:00)
// selectedBelowHotSpell — host-evaluated selectedWeather < Weather::HotSpell
// isCity — host-evaluated ZONE_TYPE::CITY mask
// true  → force selectedWeather = Fog + clamp next update to end of fog window
// false → leave selected weather (band result unchanged)
//
// Dual-wire of Go zone.ShouldForceMorningFog.
// Call site: CZone::UpdateWeather — after SelectWeatherBand selection; host
// injects fog-window, HotSpell comparison, and CITY mask before SetWeather.
// Residual 1363 pins remain in test_zone_policy_1363; dedicated dual-wire
// suite is test_zone_morning_fog_3053.
// Sibling dual-wire: ShouldRescheduleDynamicWeather (3074; timer wake).
// Sibling weather dual-wires: ShouldRejectInvalidWeather (3019),
// ShouldSkipSameWeather (3028).
inline auto ShouldForceMorningFog(
    const bool inFogWindow,
    const bool selectedBelowHotSpell,
    const bool isCity) -> bool
{
    return inFogWindow && selectedBelowHotSpell && !isCity;
}

// ShouldRescheduleDynamicWeather mirrors !weather().isStatic() on timer wake.
//
// Formula (slice 3074 dual-wire):
//
//   !isStatic
//
// isStatic — host-evaluated weather().isStatic() for this zone
// true  → re-arm UpdateWeather after the scheduler yield (dynamic weather)
// false → leave weather static; do not call UpdateWeather again on wake
//
// Dual-wire of Go zone.ShouldRescheduleDynamicWeather.
// Call site: CZone::UpdateWeather — after SetWeather / OnZoneWeatherChange,
// the posted main-thread task injects weather().isStatic() on wake; on true
// calls UpdateWeather() again.
// Residual 1363 pins remain in test_zone_policy_1363; dedicated dual-wire
// suite is test_zone_reschedule_weather_3074.
// Sibling weather dual-wires: ShouldForceMorningFog (3053),
// ShouldRejectInvalidWeather (3019), ShouldSkipSameWeather (3028).
inline auto ShouldRescheduleDynamicWeather(const bool isStatic) -> bool
{
    return !isStatic;
}

// WeatherUpdateDelayMin/MaxExclusive for random 180..1801 seconds.
constexpr int WeatherUpdateDelayMinSeconds          = 180;
constexpr int WeatherUpdateDelayMaxExclusiveSeconds = 1801;

} // namespace zonehelpers
