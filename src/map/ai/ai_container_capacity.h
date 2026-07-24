#pragma once

#include "common/cbasetypes.h"

// Pure CAIContainer controller-dispatch gates extracted so native tests can
// pin policy without entity/controller instances, state objects, or packets.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2947: CanDispatch residual dual-wire expand
//           (controller presence outer gate for public
//           Cast/Engage/ChangeTarget/Disengage/WeaponSkill/Ability/
//           RangedAttack and typed MobSkill/PetSkill/UseItem)
//   - 2952: CanChangeState residual dual-wire expand
//           (current-state change gate for external means)
//   - 3222: CanDispatch prior dedicated dual-wire expand residual 2947
//           (hasController identity; residual expand 2947 / pure 1189)
//   - 3272: CanChangeState prior dedicated dual-wire expand residual 2952
//           (!hasCurrentState || currentCanChange; pure 1189)
//   - 3303: CanChangeState dedicated dual-wire
//           (!hasCurrentState || currentCanChange; residual expand 2952;
//           prior dedicated 3272; pure 1189)
//   - 3369: CanDispatch prior dedicated dual-wire expand residual 2947
//           (hasController identity; residual expand 2947; prior dedicated
//           3222; pure 1189)
//   - 3416: CanDispatch prior dedicated dual-wire expand residual 2947
//           (hasController identity; residual expand 2947; prior dedicated
//           3369 / 3222; pure 1189)
//   - 3470: CanDispatch prior dedicated dual-wire expand residual 2947
//           (hasController identity; residual expand 2947; prior dedicated
//           3416 / 3369 / 3222; pure 1189)
//   - 3531: CanDispatch dedicated dual-wire expand residual 2947
//           (hasController identity; residual expand 2947; prior dedicated
//           3470 / 3416 / 3369 / 3222; pure 1189)
//   - 6291: InternalEngageForceAttackAllowed +
//           InternalEngageShouldResumeInactive
//           (not-yet-engaged ForceChangeState<CAttackState> admission OR and
//           post-OnEngage prevent-action inactive resume; pure inject gates)
//   - 6292: InternalEngageIsAlreadyEngagedPath +
//           InternalEngageShouldRetarget
//           (already-engaged path selection and retarget decision; pure inject)
//
// Production host: CAIContainer::{Cast,Engage,...} (ai_container.cpp) inject
// Controller / typed dynamic_cast presence into CanDispatch before invoking
// the controller method. CAIContainer::CanChangeState injects current-state
// presence and current->CanChangeState() into CanChangeState.
// CAIContainer::Internal_Engage (not-yet-engaged path) injects CanChangeState,
// current-state completed presence, and HasPreventActionEffect(true) into
// InternalEngageForceAttackAllowed; after successful ForceChangeState +
// OnEngage it injects HasPreventActionEffect(true) into
// InternalEngageShouldResumeInactive before Inactive(0ms, false).
// CAIContainer::Internal_Engage (already-engaged path) injects battle-entity
// presence and IsEngaged into InternalEngageIsAlreadyEngagedPath, then
// GetBattleTargetID vs requested into InternalEngageShouldRetarget before
// optional ChangeTarget.
// Go dual-wire: aicontainer.CanDispatch (can_dispatch.go),
// aicontainer.CanChangeState (can_change_state.go),
// aicontainer.InternalEngageForceAttackAllowed /
// aicontainer.InternalEngageShouldResumeInactive /
// aicontainer.InternalEngageIsAlreadyEngagedPath /
// aicontainer.InternalEngageShouldRetarget
// (internal_engage.go). Prior pure port: slice 1189.

namespace aicontainerhelpers
{

// CanDispatch reports whether a controller is present to forward into.
// Mirrors the outer gate of CAIContainer::{Cast,Engage,...}:
//
//   if (Controller) { return Controller->X(...); }
//   return false;
//
// Formula (slice 3531 dedicated dual-wire expand residual 2947; prior dedicated
// 3470 / 3416 / 3369 / 3222 / residual 2947 / pure 1189 — formula unchanged):
//   hasController
//
// hasController — host-evaluated controller presence:
//   base actions: Controller != nullptr (unique_ptr bool)
//   MobSkill / PetSkill / UseItem: matching dynamic_cast present
// true  → host may invoke controller method and return its result
// false → host returns false without invoking
//
// Dual-wire of Go aicontainer.CanDispatch
// (internal/aicontainer/can_dispatch.go).
// Call site: CAIContainer public controller-forward methods.
// Prior pure port: slice 1189. Residual dual-wire suite: 2947 /
// test_aicontainer_can_dispatch_2947. Prior dedicated dual-wire suites:
// 3222 / test_aicontainer_can_dispatch_3222 (retained);
// 3369 / test_aicontainer_can_dispatch_3369 (retained);
// 3416 / test_aicontainer_can_dispatch_3416 (retained);
// 3470 / test_aicontainer_can_dispatch_3470 (retained). Dedicated dual-wire
// suite is test_aicontainer_can_dispatch_3531. Formula is unchanged; this
// slice only expands dual-wire docs + index + dedicated suite.
// Sibling dual-wire left alone: 2952 / 3272 / 3303 CanChangeState.
inline auto CanDispatch(const bool hasController) -> bool
{
    return hasController;
}

// CanChangeState reports whether the AI may change state from external means.
// Mirrors CAIContainer::CanChangeState:
//
//   return !GetCurrentState() || GetCurrentState()->CanChangeState();
//
// Formula (slice 3303 dedicated dual-wire expand residual 2952; prior dedicated
// 3272 / residual 2952 / pure 1189 — formula unchanged):
//   !hasCurrentState || currentCanChange
//
// hasCurrentState — host-evaluated current-state presence:
//   GetCurrentState() != nullptr
// currentCanChange — host-injected current state's CanChangeState() result
//   (ignored when idle / no current state)
// true  → host may change state from external means
// false → host must not change state (current state blocks)
//
// Dual-wire of Go aicontainer.CanChangeState
// (internal/aicontainer/can_change_state.go).
// Call site: CAIContainer::CanChangeState.
// Prior pure port: slice 1189. Residual dual-wire suite: 2952 /
// test_aicontainer_can_change_state_2952. Prior dedicated dual-wire suite:
// 3272 / test_aicontainer_can_change_state_3272 (retained). Dedicated dual-wire
// suite is test_aicontainer_can_change_state_3303. Formula is unchanged; this
// slice only expands dual-wire docs + index + dedicated suite.
// Sibling dual-wire left alone: 2947 / 3222 / 3369 / 3416 / 3470 / 3531 CanDispatch.
inline auto CanChangeState(const bool hasCurrentState, const bool currentCanChange) -> bool
{
    return !hasCurrentState || currentCanChange;
}

// InternalEngageForceAttackAllowed reports whether the not-yet-engaged path of
// CAIContainer::Internal_Engage may attempt ForceChangeState<CAttackState>.
// Mirrors the admission OR:
//
//   CanChangeState()
//     || (GetCurrentState() && GetCurrentState()->IsCompleted())
//     || StatusEffectContainer->HasPreventActionEffect(true)
//
// Formula (slice 6291):
//   canChangeState
//     || (hasCurrentState && currentIsCompleted)
//     || hasPreventActionIgnoringCharm
//
// canChangeState — host CAIContainer::CanChangeState() (external-change gate)
// hasCurrentState — GetCurrentState() != nullptr
// currentIsCompleted — GetCurrentState()->IsCompleted() (ignored when idle)
// hasPreventActionIgnoringCharm — HasPreventActionEffect(true)
//   (allow a very brief attack-state switch so the entity is properly engaged
//   even while prevent-action status is active)
// true  → host may ForceChangeState<CAttackState>
// false → host skips ForceChangeState / OnEngage for this call
//
// Dual-wire of Go aicontainer.InternalEngageForceAttackAllowed
// (internal/aicontainer/internal_engage.go).
// Call site: CAIContainer::Internal_Engage not-yet-engaged branch.
// Already-engaged retarget / ChangeTarget, ForceChangeState object graph,
// OnEngage entity mutations, and live status-container membership remain host.
// Sibling dual-wires left alone: CanDispatch / CanChangeState free functions.
inline auto InternalEngageForceAttackAllowed(
    const bool canChangeState,
    const bool hasCurrentState,
    const bool currentIsCompleted,
    const bool hasPreventActionIgnoringCharm) -> bool
{
    return canChangeState || (hasCurrentState && currentIsCompleted) || hasPreventActionIgnoringCharm;
}

// InternalEngageShouldResumeInactive reports whether, after a successful
// ForceChangeState<CAttackState> + OnEngage, Internal_Engage must resume
// Inactive(0ms, false) because a prevent-action effect (ignoring charm) is
// still present.
//
// Formula (slice 6291):
//   hasPreventActionIgnoringCharm
//
// hasPreventActionIgnoringCharm — post-OnEngage HasPreventActionEffect(true)
// true  → host calls PAI->Inactive(0ms, false)
// false → host leaves the entity in the attack state
//
// Dual-wire of Go aicontainer.InternalEngageShouldResumeInactive
// (internal/aicontainer/internal_engage.go). Identity inject keeps production
// and tests on one pure surface (same pattern as CanDispatch).
// Call site: CAIContainer::Internal_Engage after OnEngage.
inline auto InternalEngageShouldResumeInactive(const bool hasPreventActionIgnoringCharm) -> bool
{
    return hasPreventActionIgnoringCharm;
}

// InternalEngageIsAlreadyEngagedPath reports whether CAIContainer::Internal_Engage
// takes the already-engaged branch (retarget / early return) instead of the
// not-yet-engaged ForceChangeState path.
// Mirrors the path selection:
//
//   entity && entity->PAI->IsEngaged()
//
// Formula (slice 6292):
//   hasBattleEntity && isEngaged
//
// hasBattleEntity — dynamic_cast<CBattleEntity*>(PEntity) != nullptr
// isEngaged — host PAI->IsEngaged() (inject false when no battle entity)
// true  → host takes already-engaged retarget branch
// false → host continues to not-yet-engaged path (or returns false if no entity)
//
// Dual-wire of Go aicontainer.InternalEngageIsAlreadyEngagedPath
// (internal/aicontainer/internal_engage.go).
// Call site: CAIContainer::Internal_Engage entry path selection.
// ChangeTarget body / SetBattleTargetID mutations, not-yet-engaged
// ForceChangeState/OnEngage (6291), pet engage TODOs, and full PAI ownership
// remain host/deferred.
// Sibling dual-wires left alone: CanDispatch / CanChangeState /
// InternalEngageForceAttackAllowed / InternalEngageShouldResumeInactive.
inline auto InternalEngageIsAlreadyEngagedPath(const bool hasBattleEntity, const bool isEngaged) -> bool
{
    return hasBattleEntity && isEngaged;
}

// InternalEngageShouldRetarget reports whether, on the already-engaged path of
// CAIContainer::Internal_Engage, the host must ChangeTarget(requested) and
// return true.
// Mirrors:
//
//   entity->GetBattleTargetID() != targetid
//
// Formula (slice 6292):
//   currentBattleTargetID != requestedTargetID
//
// true  → host ChangeTarget(requestedTargetID); return true
// false → host return false without ChangeTarget
//
// Dual-wire of Go aicontainer.InternalEngageShouldRetarget
// (internal/aicontainer/internal_engage.go).
// Call site: CAIContainer::Internal_Engage already-engaged branch.
// ChangeTarget controller dispatch and battle-target mutations remain host.
inline auto InternalEngageShouldRetarget(const uint16 currentBattleTargetID, const uint16 requestedTargetID) -> bool
{
    return currentBattleTargetID != requestedTargetID;
}

} // namespace aicontainerhelpers
