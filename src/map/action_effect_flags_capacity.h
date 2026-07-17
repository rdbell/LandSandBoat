#pragma once

// Pure free-function capacity for CBattleEntity::processActionEffectFlags
// (battle_entity.cpp). Residual pure ports: slices 1631 / 1709 / 2306.
//
// Dual-wire expansions of individual free functions (OmegaXI pure dual-wire):
//   - 3044: ShouldDelActorOnAttack residual dual-wire expand
//   - 3178: ShouldDelActorOnAttack prior dedicated dual-wire expand residual 3044
//           (emittedHostile identity → actor OnAttack strip;
//            residual expand 3044 / pure 1631 / 1709 / 2306)
//   - 3401: ShouldDelActorOnAttack prior dedicated dual-wire expand residual 3044
//           (prior dedicated 3178; pure 1631 / 1709 / 2306 — formula unchanged)
//   - 3453: ShouldDelActorOnAttack prior dedicated dual-wire expand residual 3044
//           (prior dedicated 3401 / 3178; pure 1631 / 1709 / 2306 — formula unchanged)
//   - 3521: ShouldDelActorOnAttack prior dedicated dual-wire expand residual 3044
//           (prior dedicated 3453 / 3401 / 3178; pure 1631 / 1709 / 2306 — formula unchanged)
//   - 3576: ShouldDelActorOnAttack prior dedicated dual-wire expand residual 3044
//           (prior dedicated 3521 / 3453 / 3401 / 3178; pure 1631 / 1709 / 2306 — formula unchanged)
//   - 3621: ShouldDelActorOnAttack prior dedicated dual-wire expand residual 3044
//           (prior dedicated 3576 / 3521 / 3453 / 3401 / 3178; pure 1631 / 1709 / 2306 — formula unchanged)
//   - 3666: ShouldDelActorOnAttack prior dedicated dual-wire expand residual 3044
//           (prior dedicated 3621 / 3576 / 3521 / 3453 / 3401 / 3178; pure 1631 / 1709 / 2306 — formula unchanged)
//   - 3711: ShouldDelActorOnAttack dedicated dual-wire expand residual 3044
//           (prior dedicated 3666 / 3621 / 3576 / 3521 / 3453 / 3401 / 3178; pure 1631 / 1709 / 2306 — formula unchanged)
//
// Residual sibling not dual-wired in 3711/3666/3621/3576/3521/3453/3401/3178/3044: ShouldDelActorAttackFlag
// (physical hostile action categories → actor ATTACK strip).
//
// Production host: CBattleEntity::processActionEffectFlags (battle_entity.cpp)
// accumulates emittedHostile from plan.countAsHostileEmit across targets; after
// the loop injects into ShouldDelActorOnAttack; on true DelStatusEffectsByFlag
// (OnAttack) on actor. Sibling ShouldDelActorAttackFlag still gates physical
// ATTACK strip.
// Go dual-wire: aistate.ShouldDelActorOnAttack
// (internal/aistate/del_actor_on_attack.go).
// Residual dual-wire suite: 3044 / test_action_del_actor_on_attack_3044.
// Prior dedicated dual-wire suite: 3178 / test_aistate_del_actor_on_attack_3178.
// Prior dedicated dual-wire suite: 3401 / test_aistate_del_actor_on_attack_3401.
// Prior dedicated dual-wire suite: 3453 / test_aistate_del_actor_on_attack_3453.
// Prior dedicated dual-wire suite: 3521 / test_aistate_del_actor_on_attack_3521.
// Prior dedicated dual-wire suite: 3576 / test_aistate_del_actor_on_attack_3576.
// Prior dedicated dual-wire suite: 3621 / test_aistate_del_actor_on_attack_3621.
// Prior dedicated dual-wire suite: 3666 / test_aistate_del_actor_on_attack_3666.
// Dedicated dual-wire suite: 3711 / test_aistate_del_actor_on_attack_3711.

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

// ShouldDelActorOnAttack mirrors emittedHostile → DelStatusEffectsByFlag(OnAttack)
// on the action actor after processActionEffectFlags target loop.
//
// Formula (slice 3711 dedicated dual-wire expand residual 3044; prior dedicated
// 3666 / 3621 / 3576 / 3521 / 3453 / 3401 / 3178; pure 1631 / 1709 / 2306 — formula unchanged):
//   emittedHostile
//
// emittedHostile — host-accumulated OR of plan.countAsHostileEmit across targets
// true  → DelStatusEffectsByFlag(OnAttack) on actor
// false → no actor ON_ATTACK strip
//
// Dual-wire of Go aistate.ShouldDelActorOnAttack
// (internal/aistate/del_actor_on_attack.go).
// Call site: CBattleEntity::processActionEffectFlags after the target loop —
// host injects emittedHostile.
// Prior pure port: slices 1631 / 1709 / 2306 (action-effect-flags pure + wire +
// target effect policy). Residual dual-wire suite: 3044 /
// test_action_del_actor_on_attack_3044. Prior dedicated dual-wire suite: 3178 /
// test_aistate_del_actor_on_attack_3178. Prior dedicated dual-wire suite: 3401 /
// test_aistate_del_actor_on_attack_3401. Prior dedicated dual-wire suite: 3453 /
// test_aistate_del_actor_on_attack_3453. Prior dedicated dual-wire suite: 3521 /
// test_aistate_del_actor_on_attack_3521. Prior dedicated dual-wire suite: 3576 /
// test_aistate_del_actor_on_attack_3576. Prior dedicated dual-wire suite: 3621 /
// test_aistate_del_actor_on_attack_3621. Prior dedicated dual-wire suite: 3666 /
// test_aistate_del_actor_on_attack_3666. Dedicated dual-wire suite is
// test_aistate_del_actor_on_attack_3711. Formula is unchanged; this slice only
// expands dual-wire docs + index + dedicated suite.
// Residual sibling: ShouldDelActorAttackFlag (physical ATTACK strip; not dual-
// wired in 3711/3666/3621/3576/3521/3453/3401/3178/3044).
inline auto ShouldDelActorOnAttack(const bool emittedHostile) -> bool
{
    return emittedHostile;
}

// ShouldDelActorAttackFlag mirrors physical hostile action categories stripping ATTACK.
// Residual pure surface (not dual-wired in 3711/3666/3621/3576/3521/3453/3401/3178/3044; sibling of ShouldDelActorOnAttack).
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
