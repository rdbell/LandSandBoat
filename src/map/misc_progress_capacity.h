#pragma once

#include <cstdint>
#include <array>

// Pure AddWeaponSkillPoints, ApplyAbilityRecast share rules, arrow scavenge
// tracking, and traverser stone accrual from charutils.

namespace miscprogresshelpers
{

// --- Weapon skill points ---

// ShouldAddWeaponSkillPoints mirrors weapon present, unlockable, and not yet unlocked.
constexpr auto ShouldAddWeaponSkillPoints(const bool hasWeapon, const bool isUnlockable, const bool isUnlocked) -> bool
{
    return hasWeapon && isUnlockable && !isUnlocked;
}

// ShouldRebuildAfterWSPoints mirrors addWsPoints returned true (weapon broke open).
constexpr auto ShouldRebuildAfterWSPoints(const bool addWsPointsResult) -> bool
{
    return addWsPointsResult;
}

// --- ApplyAbilityRecast shared timers ---

constexpr std::uint16_t RecastBloodPactRage = 173;
constexpr std::uint16_t RecastBloodPactWard = 174;
constexpr std::uint16_t RecastYonin         = 146;
constexpr std::uint16_t RecastInnin         = 147;

// HasChargeAdd mirrors charge pointer non-null.
constexpr auto HasChargeAdd(const bool hasCharge) -> bool
{
    return hasCharge;
}

// ShouldShareBloodPactTimer mirrors setting on and recast is BP rage or ward.
constexpr auto ShouldShareBloodPactTimer(const bool bloodPactSharedTimer, const std::uint16_t recastId) -> bool
{
    return bloodPactSharedTimer && (recastId == RecastBloodPactRage || recastId == RecastBloodPactWard);
}

// PairedBloodPactRecast mirrors rage ↔ ward swap.
constexpr auto PairedBloodPactRecast(const std::uint16_t recastId) -> std::uint16_t
{
    return recastId == RecastBloodPactRage ? RecastBloodPactWard : RecastBloodPactRage;
}

// ShouldMirrorYoninToInnin mirrors recastId == 146.
constexpr auto ShouldMirrorYoninToInnin(const std::uint16_t recastId) -> bool
{
    return recastId == RecastYonin;
}

struct AbilityRecastStep
{
    std::uint16_t recastId;
    bool          usesCharges;
};

struct AbilityRecastPlan
{
    std::array<AbilityRecastStep, 3> steps{};
    std::uint8_t                     count{};
};

// PlanAbilityRecast mirrors ApplyAbilityRecast's primary, shared Blood Pact,
// and Yonin/Innin timer additions. Only the primary entry carries charges.
constexpr auto PlanAbilityRecast(const std::uint16_t recastId, const bool hasCharge, const bool bloodPactSharedTimer) -> AbilityRecastPlan
{
    auto plan = AbilityRecastPlan{};
    plan.steps[0] = { recastId, hasCharge };
    plan.count = 1;

    if (ShouldShareBloodPactTimer(bloodPactSharedTimer, recastId))
    {
        plan.steps[plan.count++] = { PairedBloodPactRecast(recastId), false };
    }
    if (ShouldMirrorYoninToInnin(recastId))
    {
        plan.steps[plan.count++] = { RecastInnin, false };
    }
    return plan;
}

// --- Arrow scavenge ArrowsUsed packing ---
// Local stores ammoID * 10000 + count, count capped below 1980.

constexpr std::uint32_t ArrowsUsedScale    = 10000;
constexpr std::uint32_t ArrowsUsedCountCap = 1980;

// EncodeArrowsUsed mirrors ammoID * 10000 + 1 (initial).
constexpr auto EncodeArrowsUsed(const std::uint16_t ammoID) -> std::uint32_t
{
    return static_cast<std::uint32_t>(ammoID) * ArrowsUsedScale + 1;
}

// ArrowsUsedAmmoID mirrors floor(local / 10000).
constexpr auto ArrowsUsedAmmoID(const std::uint32_t arrowsUsedLocal) -> std::uint32_t
{
    return arrowsUsedLocal / ArrowsUsedScale;
}

// ArrowsUsedCount mirrors floor(local % 10000).
constexpr auto ArrowsUsedCount(const std::uint32_t arrowsUsedLocal) -> std::uint32_t
{
    return arrowsUsedLocal % ArrowsUsedScale;
}

// ShouldInitArrowsUsed mirrors local == 0.
constexpr auto ShouldInitArrowsUsed(const std::uint32_t arrowsUsedLocal) -> bool
{
    return arrowsUsedLocal == 0;
}

// IsSameArrowAsLast mirrors floor(local/10000) == ammoID.
constexpr auto IsSameArrowAsLast(const std::uint32_t arrowsUsedLocal, const std::uint16_t ammoID) -> bool
{
    return ArrowsUsedAmmoID(arrowsUsedLocal) == static_cast<std::uint32_t>(ammoID);
}

// ShouldIncrementArrowsUsed mirrors count < 1980 (not >= 1980).
constexpr auto ShouldIncrementArrowsUsed(const std::uint32_t arrowsUsedLocal) -> bool
{
    return ArrowsUsedCount(arrowsUsedLocal) < ArrowsUsedCountCap;
}

// IncrementArrowsUsed mirrors local + 1.
constexpr auto IncrementArrowsUsed(const std::uint32_t arrowsUsedLocal) -> std::uint32_t
{
    return arrowsUsedLocal + 1;
}

// --- Traverser stone accrual ---

constexpr std::uint32_t TraverserBaseWaitHours      = 20;
constexpr std::uint32_t TraverserCelerityHoursEach  = 4;
constexpr std::uint8_t  TraverserCelerityKeyItemMax = 3; // azure/crimson/ivory

// IsTraverserEpochUnset mirrors epoch == 0 seconds.
constexpr auto IsTraverserEpochUnset(const std::uint32_t epochUnixSeconds) -> bool
{
    return epochUnixSeconds == 0;
}

// TraverserWaitHours mirrors 20h - 4h * celerityKICount (clamped count).
constexpr auto TraverserWaitHours(const std::uint8_t celerityKeyItemCount) -> std::uint32_t
{
    auto count = celerityKeyItemCount;
    if (count > TraverserCelerityKeyItemMax)
    {
        count = TraverserCelerityKeyItemMax;
    }
    return TraverserBaseWaitHours - TraverserCelerityHoursEach * static_cast<std::uint32_t>(count);
}

// TraverserStonesGenerated mirrors floor(elapsedHours / waitHours).
// Host supplies elapsed hours since epoch (floor to whole hours).
constexpr auto TraverserStonesGenerated(const std::uint32_t elapsedHours, const std::uint32_t waitHours) -> std::uint32_t
{
    if (waitHours == 0)
    {
        return 0;
    }
    return elapsedHours / waitHours;
}

// AvailableTraverserStones mirrors generated - claimed (unsigned wrap if claimed > generated).
constexpr auto AvailableTraverserStones(const std::uint32_t generated, const std::uint32_t claimed) -> std::uint32_t
{
    return generated - claimed;
}

} // namespace miscprogresshelpers
