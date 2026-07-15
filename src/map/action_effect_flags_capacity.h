#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

namespace actioneffectflagshelpers
{

// ActionCategory pins used by processActionEffectFlags ATTACK-flag strip.
constexpr uint8 ActionCategoryBasicAttack    = 1;
constexpr uint8 ActionCategorySkillFinish    = 3;
constexpr uint8 ActionCategoryMobSkillFinish = 11;
constexpr uint8 ActionCategoryPetSkillFinish = 13;

// TargetEffectPlan is the pure per-target side-effect plan inside the action loop.
struct TargetEffectPlan
{
    bool skipEvent{ false };           // PC in event → skipEvent
    bool delDetectable{ false };       // main hostile target
    bool delOnAttack{ false };         // every hostile target
    bool interruptFishing{ false };    // PC fishing
    bool countAsHostileEmit{ false };  // different allegiance
};

// ResolveTargetEffectPlan mirrors one processActionEffectFlags target iteration.
// isMainTarget is true only for the first target in the action list.
inline auto ResolveTargetEffectPlan(
    const bool hasTarget,
    const bool isPC,
    const bool inEvent,
    const bool isFishing,
    const bool differentAllegiance,
    const bool isMainTarget) -> TargetEffectPlan
{
    TargetEffectPlan plan{};
    if (!hasTarget)
    {
        return plan;
    }

    if (isPC && inEvent)
    {
        plan.skipEvent = true;
    }

    if (differentAllegiance)
    {
        plan.countAsHostileEmit = true;
        plan.delOnAttack        = true;
        if (isMainTarget)
        {
            plan.delDetectable = true;
        }
        if (isPC && isFishing)
        {
            plan.interruptFishing = true;
        }
    }
    return plan;
}

// ShouldDelActorOnAttack mirrors emittedHostile → DelStatusEffectsByFlag(OnAttack) on actor.
inline auto ShouldDelActorOnAttack(const bool emittedHostile) -> bool
{
    return emittedHostile;
}

// ShouldDelActorAttackFlag mirrors physical hostile action categories stripping ATTACK.
inline auto ShouldDelActorAttackFlag(const bool emittedHostile, const uint8 actionType) -> bool
{
    if (!emittedHostile)
    {
        return false;
    }
    return actionType == ActionCategoryBasicAttack ||
           actionType == ActionCategorySkillFinish ||
           actionType == ActionCategoryMobSkillFinish ||
           actionType == ActionCategoryPetSkillFinish;
}

// IsPhysicalHostileActionType is the category subset used by ShouldDelActorAttackFlag.
inline auto IsPhysicalHostileActionType(const uint8 actionType) -> bool
{
    return actionType == ActionCategoryBasicAttack ||
           actionType == ActionCategorySkillFinish ||
           actionType == ActionCategoryMobSkillFinish ||
           actionType == ActionCategoryPetSkillFinish;
}

// NextIsMainTarget mirrors isMainTarget = false after the first target.
inline auto NextIsMainTarget() -> bool
{
    return false;
}

// InitialIsMainTarget is true before the first target is processed.
inline auto InitialIsMainTarget() -> bool
{
    return true;
}

} // namespace actioneffectflagshelpers
