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
//   - 6294: InternalChangeTargetHasBattleEntity +
//           InternalChangeTargetShouldSetBattleTarget
//           (Internal_ChangeTarget outer battle-entity gate and
//           IsEngaged||targetid==0 path split; pure inject)
//   - 6296: InternalDisengageHasBattleEntity
//           (Internal_Disengage outer battle-entity gate; pure inject)
//   - 6298: InternalDieHasBattleEntity
//           (Internal_Die outer battle-entity gate; pure inject)
//   - 6300: InternalDespawnAllowed
//           (Internal_Despawn not-already-despawning admission; pure inject)
//   - 6302: InternalActionTargetAllowed
//           (Internal_Cast/WeaponSkill/MobSkill/PetSkill/Ability/RangedAttack
//           target untargetable gate; pure inject)
//   - 6303: AcceptRaiseShouldInvoke
//           (Accept_Raise death-state admission; pure inject)
//   - 6304: InternalSynthAllowed
//           (Internal_Synth char-entity + not-already-synth admission; pure inject)
//   - 6305: DespawnShouldDispatchController
//           (public Despawn() controller-vs-Internal_Despawn branch; pure inject)
//   - 6306: CanFollowPath
//           (PathFind && CanChangeState injects; pure dual-wire residual 1189)
//   - 6307: InternalUseItemHasCharEntity
//           (Internal_UseItem outer char-entity gate; pure inject)
//   - 6308: IsEngagedAnimation / IsRoamingAnimation
//           (IsEngaged / IsRoaming animation identity dual-wire residual 1189)
//   - 6309: IsSpawnedStatus
//           (IsSpawned !isDisappear dual-wire; SHUTDOWN not rejected)
//   - 6310: IsUntargetable
//           (inactive-state && inactive flag) || entity flag dual-wire residual 1189
//   - 6311: IsStateStackEmpty
//           (!hasCurrentState dual-wire residual pure 1189)
//   - 6312: CanPushState
//           (stateCount <= 10 stack ceiling dual-wire residual pure 1189)
//   - 6313: TickStateLoopContinue
//           (guard <= 32 Tick state-drain loop bound dual-wire residual pure 1189)
//   - 6314: TickPreventActionParkAllowed
//           (Tick post-drain prevent-action → Inactive park admission AND)
//   - 6321: ShouldInterruptCurrent
//           (InterruptStates while current&&CanInterrupt loop admission)
//   - 6322: ShouldClearCurrentState / ShouldCleanupCompletedState
//           (ClearStateStack / CheckCompletedStates loop admissions)
//   - 6323: IsActionQueueEmpty
//           (QueueEmpty ActionQueue.isEmpty identity inject)
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
// CAIContainer::Internal_ChangeTarget injects battle-entity presence into
// InternalChangeTargetHasBattleEntity, then IsEngaged and targetid into
// InternalChangeTargetShouldSetBattleTarget before SetBattleTargetID vs Engage.
// CAIContainer::Internal_Disengage injects battle-entity presence into
// InternalDisengageHasBattleEntity before SetBattleTargetID(0).
// CAIContainer::Internal_Die injects battle-entity presence into
// InternalDieHasBattleEntity before ChangeState<CDeathState>.
// CAIContainer::Internal_Despawn injects IsCurrentState<CDespawnState> into
// InternalDespawnAllowed before ForceChangeState<CDespawnState>.
// CAIContainer::Internal_Cast / Internal_WeaponSkill / Internal_MobSkill /
// Internal_PetSkill / Internal_Ability / Internal_RangedAttack inject GetEntity
// presence and PAI->IsUntargetable into InternalActionTargetAllowed before
// ChangeState.
// CAIContainer::Accept_Raise injects IsCurrentState<CDeathState> into
// AcceptRaiseShouldInvoke before acceptRaise().
// CAIContainer::Internal_Synth injects char-entity presence and
// IsCurrentState<CSynthState> into InternalSynthAllowed before
// ForceChangeState<CSynthState>.
// CAIContainer::Despawn injects Controller presence into
// DespawnShouldDispatchController before Controller->Despawn vs Internal_Despawn.
// CAIContainer::CanFollowPath injects PathFind presence and CanChangeState
// injects into CanFollowPath.
// CAIContainer::Internal_UseItem injects char-entity presence into
// InternalUseItemHasCharEntity before ChangeState<CItemState>.
// CAIContainer::IsEngaged / IsRoaming inject animation comparisons into
// IsEngagedAnimation / IsRoamingAnimation.
// CAIContainer::IsSpawned injects DISAPPEAR comparison into IsSpawnedStatus.
// CAIContainer::IsUntargetable injects inactive-state presence, inactive
// GetUntargetable, and entity GetUntargetable into IsUntargetable.
// CAIContainer::IsStateStackEmpty injects current-state presence into
// IsStateStackEmpty.
// CAIContainer::ChangeState / ForceChangeState inject stateCount() into
// CanPushState for the stack ceiling gate.
// CAIContainer::Tick injects post-increment guard into TickStateLoopContinue.
// CAIContainer::Tick post-drain injects battle/alive/state/prevent-action
// predicates into TickPreventActionParkAllowed before Inactive(0ms, false).
// CAIContainer::InterruptStates injects current-state presence and
// CanInterrupt into ShouldInterruptCurrent for the while-loop admission.
// CAIContainer::ClearStateStack injects current-state presence into
// ShouldClearCurrentState. CAIContainer::CheckCompletedStates injects
// current-state presence and IsCompleted into ShouldCleanupCompletedState.
// CAIContainer::QueueEmpty injects ActionQueue.isEmpty into IsActionQueueEmpty.
// Go dual-wire: aicontainer.CanDispatch (can_dispatch.go),
// aicontainer.CanChangeState (can_change_state.go),
// aicontainer.CanFollowPath (aicontainer.go),
// aicontainer.InternalEngageForceAttackAllowed /
// aicontainer.InternalEngageShouldResumeInactive /
// aicontainer.InternalEngageIsAlreadyEngagedPath /
// aicontainer.InternalEngageShouldRetarget
// (internal_engage.go),
// aicontainer.InternalChangeTargetHasBattleEntity /
// aicontainer.InternalChangeTargetShouldSetBattleTarget
// (internal_change_target.go),
// aicontainer.InternalDisengageHasBattleEntity
// (internal_disengage.go),
// aicontainer.InternalDieHasBattleEntity
// (internal_die.go),
// aicontainer.InternalDespawnAllowed
// (internal_despawn.go),
// aicontainer.InternalActionTargetAllowed
// (internal_action_target.go),
// aicontainer.AcceptRaiseShouldInvoke
// (accept_raise.go),
// aicontainer.InternalSynthAllowed
// (internal_synth.go),
// aicontainer.DespawnShouldDispatchController
// (despawn_dispatch.go),
// aicontainer.CanFollowPath
// (aicontainer.go),
// aicontainer.InternalUseItemHasCharEntity
// (internal_use_item.go),
// aicontainer.IsEngagedAnimation / aicontainer.IsRoamingAnimation
// (animation_status.go),
// aicontainer.IsSpawnedStatus
// (is_spawned.go),
// aicontainer.IsUntargetable
// (aicontainer.go),
// aicontainer.IsStateStackEmpty
// (aicontainer.go),
// aicontainer.CanPushState
// (aicontainer.go),
// aicontainer.TickStateLoopContinue
// (aicontainer.go),
// aicontainer.TickPreventActionParkAllowed
// (tick_prevent_action_park.go),
// aicontainer.ShouldInterruptCurrent
// (interrupt_states.go),
// aicontainer.ShouldClearCurrentState /
// aicontainer.ShouldCleanupCompletedState
// (state_stack_cleanup.go),
// aicontainer.IsActionQueueEmpty
// (queue_empty.go). Prior pure port: slice 1189.

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
// CanFollowPath (6306) composes this free function with PathFind presence.
inline auto CanChangeState(const bool hasCurrentState, const bool currentCanChange) -> bool
{
    return !hasCurrentState || currentCanChange;
}

// CanFollowPath reports whether the AI may follow a path under the current
// state constraints.
// Mirrors CAIContainer::CanFollowPath:
//
//   return PathFind && (!GetCurrentState() || GetCurrentState()->CanChangeState());
//
// Formula (slice 6306 dedicated dual-wire residual pure 1189):
//   hasPathFind && CanChangeState(hasCurrentState, currentCanChange)
//   // ≡ hasPathFind && (!hasCurrentState || currentCanChange)
//
// hasPathFind — PathFind != nullptr
// hasCurrentState / currentCanChange — same injects as CanChangeState (3303)
// true  → host may pathfind under current state
// false → host must not follow path (no PathFind or current state blocks)
//
// Dual-wire of Go aicontainer.CanFollowPath
// (internal/aicontainer/aicontainer.go). Host injects PathFind presence and
// CanChangeState injects so production and tests share one pure surface.
// Call site: CAIContainer::CanFollowPath. Prior pure port: slice 1189.
// Sibling dual-wires left alone: CanChangeState / CanDispatch and Internal_*.
inline auto CanFollowPath(const bool hasPathFind, const bool hasCurrentState, const bool currentCanChange) -> bool
{
    return hasPathFind && CanChangeState(hasCurrentState, currentCanChange);
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

// InternalChangeTargetHasBattleEntity reports whether CAIContainer::Internal_ChangeTarget
// may proceed past the outer dynamic_cast gate.
// Mirrors:
//
//   auto* entity = dynamic_cast<CBattleEntity*>(PEntity);
//   if (entity) { ... }
//
// Formula (slice 6294):
//   hasBattleEntity
//
// hasBattleEntity — dynamic_cast<CBattleEntity*>(PEntity) != nullptr
// true  → host evaluates path split (SetBattleTargetID vs Engage)
// false → host returns false without path split
//
// Dual-wire of Go aicontainer.InternalChangeTargetHasBattleEntity
// (internal/aicontainer/internal_change_target.go). Identity inject keeps
// production and tests on one pure surface (same pattern as CanDispatch).
// Call site: CAIContainer::Internal_ChangeTarget entry outer gate.
// SetBattleTargetID mutations, Engage controller body, public ChangeTarget
// CanDispatch residual, and full PAI ownership remain host/deferred.
// Sibling dual-wires left alone: CanDispatch / CanChangeState /
// InternalEngage* free functions.
inline auto InternalChangeTargetHasBattleEntity(const bool hasBattleEntity) -> bool
{
    return hasBattleEntity;
}

// InternalChangeTargetShouldSetBattleTarget reports whether, once
// Internal_ChangeTarget has a battle entity, the host must SetBattleTargetID
// and return true instead of Engage(targetid).
// Mirrors:
//
//   IsEngaged() || targetid == 0
//
// Formula (slice 6294):
//   isEngaged || targetid == 0
//
// isEngaged — host CAIContainer::IsEngaged()
// targetid — requested battle target id (0 clears / sets zero without Engage)
// true  → host SetBattleTargetID(targetid); return true
// false → host return Engage(targetid)
//
// Dual-wire of Go aicontainer.InternalChangeTargetShouldSetBattleTarget
// (internal/aicontainer/internal_change_target.go).
// Call site: CAIContainer::Internal_ChangeTarget path split.
// SetBattleTargetID mutations and Engage controller body remain host.
// Sibling dual-wires left alone: CanDispatch / CanChangeState /
// InternalEngage* free functions.
inline auto InternalChangeTargetShouldSetBattleTarget(const bool isEngaged, const uint16 targetid) -> bool
{
    return isEngaged || targetid == 0;
}

// InternalDisengageHasBattleEntity reports whether CAIContainer::Internal_Disengage
// may proceed past the outer dynamic_cast gate.
// Mirrors:
//
//   auto* entity = dynamic_cast<CBattleEntity*>(PEntity);
//   if (entity) { ... }
//
// Formula (slice 6296):
//   hasBattleEntity
//
// hasBattleEntity — dynamic_cast<CBattleEntity*>(PEntity) != nullptr
// true  → host SetBattleTargetID(0); return true
// false → host returns false without mutation
//
// Dual-wire of Go aicontainer.InternalDisengageHasBattleEntity
// (internal/aicontainer/internal_disengage.go). Identity inject keeps
// production and tests on one pure surface (same pattern as CanDispatch /
// InternalChangeTargetHasBattleEntity).
// Call site: CAIContainer::Internal_Disengage entry outer gate.
// SetBattleTargetID mutation semantics, public Disengage CanDispatch residual,
// pet engage/disengage TODOs, and full PAI ownership remain host/deferred.
// Sibling dual-wires left alone: CanDispatch / CanChangeState /
// InternalEngage* / InternalChangeTarget* free functions.
inline auto InternalDisengageHasBattleEntity(const bool hasBattleEntity) -> bool
{
    return hasBattleEntity;
}

// InternalDieHasBattleEntity reports whether CAIContainer::Internal_Die
// may proceed past the outer dynamic_cast gate.
// Mirrors:
//
//   auto* entity = dynamic_cast<CBattleEntity*>(PEntity);
//   if (entity) { ... }
//
// Formula (slice 6298):
//   hasBattleEntity
//
// hasBattleEntity — dynamic_cast<CBattleEntity*>(PEntity) != nullptr
// true  → host ChangeState<CDeathState>(entity, deathTime); return its result
// false → host returns false without ChangeState
//
// Dual-wire of Go aicontainer.InternalDieHasBattleEntity
// (internal/aicontainer/internal_die.go). Identity inject keeps production
// and tests on one pure surface (same pattern as CanDispatch /
// InternalDisengageHasBattleEntity).
// Call site: CAIContainer::Internal_Die entry outer gate.
// ChangeState/ForceChangeState object graph, CDeathState construction
// side-effects (already 6295), deathTime selection hosts, and full PAI
// ownership remain host/deferred. Sibling dual-wires left alone:
// CanDispatch / CanChangeState / InternalEngage* / InternalChangeTarget* /
// InternalDisengage* free functions. Internal_Despawn (6300), Accept_Raise,
// and skill Internal_* untargetable checks (6302) are separate slices.
inline auto InternalDieHasBattleEntity(const bool hasBattleEntity) -> bool
{
    return hasBattleEntity;
}

// InternalDespawnAllowed reports whether CAIContainer::Internal_Despawn
// may ForceChangeState into CDespawnState.
// Mirrors:
//
//   if (!IsCurrentState<CDespawnState>()) {
//       return ForceChangeState<CDespawnState>(PEntity, instantDespawn);
//   }
//   return false;
//
// Formula (slice 6300):
//   !isCurrentDespawnState
//
// isCurrentDespawnState — host IsCurrentState<CDespawnState>()
// true  → already despawning; host returns false without ForceChangeState
// false → host ForceChangeState<CDespawnState>(PEntity, instantDespawn);
//         return its result
//
// Dual-wire of Go aicontainer.InternalDespawnAllowed
// (internal/aicontainer/internal_despawn.go). Host injects the current-state
// type check so production and tests share one pure surface (same pattern as
// CanDispatch / InternalDieHasBattleEntity).
// Call site: CAIContainer::Internal_Despawn outer admission.
// ForceChangeState/enterState object graph, CDespawnState ctor/Update
// (already 0770/6299), public Despawn() controller-vs-Internal_Despawn
// branch, Accept_Raise, skill Internal_* untargetable checks (6302), Tick
// prevent-action park, and full PAI ownership remain host/deferred.
// Sibling dual-wires left alone: CanDispatch / CanChangeState /
// InternalEngage* / InternalChangeTarget* / InternalDisengage* /
// InternalDie* free functions.
inline auto InternalDespawnAllowed(const bool isCurrentDespawnState) -> bool
{
    return !isCurrentDespawnState;
}

// InternalActionTargetAllowed reports whether skill/spell Internal_* paths
// may proceed past the target untargetable check after the outer entity
// dynamic_cast gate has already passed.
// Mirrors the shared pattern in:
//
//   Internal_Cast / Internal_WeaponSkill / Internal_MobSkill /
//   Internal_PetSkill / Internal_Ability / Internal_RangedAttack
//
//   if (auto* target = entity->GetEntity(targid); target && target->PAI->IsUntargetable()) {
//       return false;
//   }
//   return ChangeState<...>(...);
//
// Formula (slice 6302):
//   !hasTarget || !isUntargetable
//   (equivalent to !(hasTarget && isUntargetable); De Morgan form matches
//   Go static-analysis dual-wire)
//
// hasTarget — entity->GetEntity(targid) != nullptr
// isUntargetable — host target->PAI->IsUntargetable() when hasTarget; host
// must inject false when there is no target (do not dereference null)
// true  → host ChangeState for the Internal_* action; return its result
// false → host returns false without ChangeState (target exists and untargetable)
//
// Dual-wire of Go aicontainer.InternalActionTargetAllowed
// (internal/aicontainer/internal_action_target.go). Host injects GetEntity
// presence and IsUntargetable so production and tests share one pure surface.
// Call sites: CAIContainer::Internal_Cast, Internal_WeaponSkill,
// Internal_MobSkill, Internal_PetSkill, Internal_Ability, Internal_RangedAttack
// after the outer entity dynamic_cast gate.
// ChangeState object graph, spell/ability/skill ID validation, pet-entity
// vs battle-entity outer casts, and full PAI ownership remain host/deferred.
// Sibling dual-wires left alone: CanDispatch / CanChangeState /
// InternalEngage* / InternalChangeTarget* / InternalDisengage* /
// InternalDie* / InternalDespawn* free functions. Accept_Raise (6303) and
// public Despawn() controller branch remain separate slices.
inline auto InternalActionTargetAllowed(const bool hasTarget, const bool isUntargetable) -> bool
{
    return !hasTarget || !isUntargetable;
}

// AcceptRaiseShouldInvoke reports whether CAIContainer::Accept_Raise may call
// CDeathState::acceptRaise on the current state.
// Mirrors:
//
//   if (IsCurrentState<CDeathState>()) {
//       static_cast<CDeathState*>(GetCurrentState())->acceptRaise();
//   }
//   return false; // always
//
// Formula (slice 6303):
//   isCurrentDeathState
//
// isCurrentDeathState — host IsCurrentState<CDeathState>()
// true  → host casts current state to CDeathState and calls acceptRaise()
// false → host skips acceptRaise
//
// Accept_Raise always returns false after the optional host side-effect;
// that residual return is host-only and not part of this pure gate.
//
// Dual-wire of Go aicontainer.AcceptRaiseShouldInvoke
// (internal/aicontainer/accept_raise.go). Host injects the current-state type
// check so production and tests share one pure surface (same pattern as
// InternalDespawnAllowed identity inject).
// Call site: CAIContainer::Accept_Raise outer admission.
// CDeathState::acceptRaise mutation (m_raiseAccepted / timers), death Update
// raise paths (already 6293), and full PAI ownership remain host/deferred.
// Sibling dual-wires left alone: CanDispatch / CanChangeState /
// InternalEngage* / InternalChangeTarget* / InternalDisengage* /
// InternalDie* / InternalDespawn* / InternalActionTarget* free functions.
// Public Despawn() controller branch and Internal_Synth (6304) remain
// separate slices.
inline auto AcceptRaiseShouldInvoke(const bool isCurrentDeathState) -> bool
{
    return isCurrentDeathState;
}

// InternalSynthAllowed reports whether CAIContainer::Internal_Synth may
// ForceChangeState into CSynthState.
// Mirrors:
//
//   auto PChar = dynamic_cast<CCharEntity*>(PEntity);
//   if (PChar && !IsCurrentState<CSynthState>()) {
//       return ForceChangeState<CSynthState>(PChar, synthSkill);
//   }
//   return false;
//
// Formula (slice 6304):
//   hasCharEntity && !isCurrentSynthState
//
// hasCharEntity — dynamic_cast<CCharEntity*>(PEntity) != nullptr
// isCurrentSynthState — host IsCurrentState<CSynthState>()
// true  → host ForceChangeState<CSynthState>(PChar, synthSkill); return result
// false → host returns false without ForceChangeState
//
// Dual-wire of Go aicontainer.InternalSynthAllowed
// (internal/aicontainer/internal_synth.go). Host injects char-entity presence
// and current-synth-state so production and tests share one pure surface
// (same pattern as InternalDespawnAllowed).
// Call site: CAIContainer::Internal_Synth outer admission.
// ForceChangeState/enterState object graph, CSynthState construction,
// synthSkill payload, and full PAI ownership remain host/deferred.
// Sibling dual-wires left alone: CanDispatch / CanChangeState /
// InternalEngage* / InternalChangeTarget* / InternalDisengage* /
// InternalDie* / InternalDespawn* / InternalActionTarget* /
// AcceptRaise* free functions. Public Despawn() controller branch (6305)
// remains a separate slice.
inline auto InternalSynthAllowed(const bool hasCharEntity, const bool isCurrentSynthState) -> bool
{
    return hasCharEntity && !isCurrentSynthState;
}

// DespawnShouldDispatchController reports whether CAIContainer::Despawn
// should forward to Controller->Despawn() rather than Internal_Despawn().
// Mirrors:
//
//   if (Controller) {
//       Controller->Despawn();
//   } else {
//       Internal_Despawn();
//   }
//
// Formula (slice 6305):
//   hasController
//
// hasController — Controller != nullptr (same inject as CanDispatch)
// true  → host Controller->Despawn()
// false → host Internal_Despawn() (default instantDespawn=false path)
//
// Dual-wire of Go aicontainer.DespawnShouldDispatchController
// (internal/aicontainer/despawn_dispatch.go). Identity inject keeps
// production and tests on one pure surface (same pattern as CanDispatch).
// Call site: CAIContainer::Despawn branch selection.
// Controller->Despawn body, Internal_Despawn admission (already 6300),
// and full PAI ownership remain host/deferred.
// Sibling dual-wires left alone: CanDispatch / CanChangeState /
// InternalEngage* / InternalChangeTarget* / InternalDisengage* /
// InternalDie* / InternalDespawn* / InternalActionTarget* /
// AcceptRaise* / InternalSynth* free functions.
inline auto DespawnShouldDispatchController(const bool hasController) -> bool
{
    return hasController;
}

// InternalUseItemHasCharEntity reports whether CAIContainer::Internal_UseItem
// may proceed past the outer dynamic_cast gate.
// Mirrors:
//
//   auto* entity = dynamic_cast<CCharEntity*>(PEntity);
//   if (entity) {
//       return ChangeState<CItemState>(entity, targetid, loc, slotid);
//   }
//   return false;
//
// Formula (slice 6307):
//   hasCharEntity
//
// hasCharEntity — dynamic_cast<CCharEntity*>(PEntity) != nullptr
// true  → host ChangeState<CItemState>(entity, targetid, loc, slotid); return result
// false → host returns false without ChangeState
//
// Dual-wire of Go aicontainer.InternalUseItemHasCharEntity
// (internal/aicontainer/internal_use_item.go). Identity inject keeps production
// and tests on one pure surface (same pattern as InternalDieHasBattleEntity).
// Call site: CAIContainer::Internal_UseItem entry outer gate.
// ChangeState object graph, CItemState construction, targetid/loc/slotid
// hosts, and full PAI ownership remain host/deferred.
// Sibling dual-wires left alone: CanDispatch / CanChangeState / CanFollowPath /
// InternalEngage* / InternalChangeTarget* / InternalDisengage* /
// InternalDie* / InternalDespawn* / InternalActionTarget* /
// AcceptRaise* / InternalSynth* / DespawnShouldDispatch* free functions.
inline auto InternalUseItemHasCharEntity(const bool hasCharEntity) -> bool
{
    return hasCharEntity;
}

// IsEngagedAnimation reports whether CAIContainer::IsEngaged is true.
// Mirrors: return PEntity->animation == ANIMATION_ATTACK;
// Formula (slice 6308): animationIsAttack
// Dual-wire of Go aicontainer.IsEngagedAnimation (animation_status.go).
// Call site: CAIContainer::IsEngaged. Prior pure port: slice 1189.
inline auto IsEngagedAnimation(const bool animationIsAttack) -> bool
{
    return animationIsAttack;
}

// IsRoamingAnimation reports whether CAIContainer::IsRoaming is true.
// Mirrors: return PEntity->animation == ANIMATION_NONE;
// Formula (slice 6308): animationIsNone
// Dual-wire of Go aicontainer.IsRoamingAnimation (animation_status.go).
// Call site: CAIContainer::IsRoaming. Prior pure port: slice 1189.
inline auto IsRoamingAnimation(const bool animationIsNone) -> bool
{
    return animationIsNone;
}

// IsSpawnedStatus reports whether CAIContainer::IsSpawned is true.
// Mirrors production:
//
//   return PEntity->status != STATUS_TYPE::DISAPPEAR;
//
// Formula (slice 6309):
//   !isDisappear
//
// isDisappear — host PEntity->status == STATUS_TYPE::DISAPPEAR
// true  → entity is considered spawned
// false → entity is despawned/disappeared
//
// Dual-wire of Go aicontainer.IsSpawnedStatus (is_spawned.go).
// Call site: CAIContainer::IsSpawned. Prior pure port (1189) incorrectly also
// rejected SHUTDOWN; production only checks DISAPPEAR. Sibling dual-wires
// left alone: IsEngagedAnimation / IsRoamingAnimation (6308).
inline auto IsSpawnedStatus(const bool isDisappear) -> bool
{
    return !isDisappear;
}

// IsUntargetable reports whether CAIContainer::IsUntargetable is true.
// Mirrors:
//
//   (IsCurrentState<CInactiveState>() && inactive.GetUntargetable())
//     || PEntity->GetUntargetable()
//
// Formula (slice 6310 dedicated dual-wire residual pure 1189):
//   (isInactiveState && inactiveUntargetable) || entityUntargetable
//
// isInactiveState — host IsCurrentState<CInactiveState>()
// inactiveUntargetable — host CInactiveState::GetUntargetable() when inactive;
// host must inject false when not inactive (do not cast null)
// entityUntargetable — host CBaseEntity::GetUntargetable()
//
// Dual-wire of Go aicontainer.IsUntargetable (aicontainer.go).
// Call site: CAIContainer::IsUntargetable. Prior pure port: slice 1189.
// Sibling dual-wires left alone: IsSpawnedStatus / IsEngagedAnimation /
// IsRoamingAnimation / InternalActionTargetAllowed free functions.
inline auto IsUntargetable(const bool isInactiveState, const bool inactiveUntargetable, const bool entityUntargetable) -> bool
{
    return (isInactiveState && inactiveUntargetable) || entityUntargetable;
}

// IsStateStackEmpty reports whether CAIContainer::IsStateStackEmpty is true.
// Mirrors:
//
//   return !m_currentState;
//
// Formula (slice 6311 dedicated dual-wire residual pure 1189):
//   !hasCurrentState
//
// hasCurrentState — host m_currentState / GetCurrentState() != nullptr
// Note: LSB names this "stack empty" but the predicate is only about the
// current state pointer, not m_stateStack size.
//
// Dual-wire of Go aicontainer.IsStateStackEmpty (aicontainer.go).
// Call site: CAIContainer::IsStateStackEmpty. Prior pure port: slice 1189.
// Sibling dual-wires left alone: CanChangeState / CanFollowPath / IsCurrentState.
inline auto IsStateStackEmpty(const bool hasCurrentState) -> bool
{
    return !hasCurrentState;
}

// CanPushState reports whether ChangeState / ForceChangeState may enter a new
// state given the current stateCount.
// Mirrors:
//
//   if (stateCount() > 10) { ShowWarning(...); return false; }
//
// Formula (slice 6312 dedicated dual-wire residual pure 1189):
//   stateCount <= 10
//
// At stateCount == 10 push is still allowed; stateCount > 10 rejects.
// Dual-wire of Go aicontainer.CanPushState (aicontainer.go; MaxStateCount=10).
// Call sites: CAIContainer::ChangeState / ForceChangeState stack ceiling.
// ShowWarning on reject remains host-only. Sibling dual-wires left alone:
// CanChangeState / IsStateStackEmpty free functions.
inline auto CanPushState(const std::size_t stateCount) -> bool
{
    return stateCount <= 10;
}

// TickStateLoopContinue reports whether the Tick state-drain loop may keep
// iterating after guard has been incremented for this pass.
// Mirrors:
//
//   if (++guard > 32) { ShowWarning(...); break; }
//
// Formula (slice 6313 dedicated dual-wire residual pure 1189):
//   guard <= 32
//
// Pass the post-increment guard value. Continues while guard <= 32; breaks at 33+.
// Dual-wire of Go aicontainer.TickStateLoopContinue (aicontainer.go;
// StateTickGuardMax=32). Call site: CAIContainer::Tick state-drain loop.
// ShowWarning on break remains host-only. Sibling dual-wires left alone:
// CanPushState / IsStateStackEmpty free functions.
inline auto TickStateLoopContinue(const int guard) -> bool
{
    return guard <= 32;
}

// TickPreventActionParkAllowed reports whether CAIContainer::Tick may park
// the entity into Inactive(0ms, false) after the state-drain loop due to a
// prevent-action status effect.
// Mirrors:
//
//   if (battle && isAlive && !Inactive && !Magic && !MobSkill && HasPreventAction) {
//       Inactive(0ms, false);
//   }
//
// Formula (slice 6314):
//   hasBattleEntity && isAlive && !isInactiveState && !isMagicState &&
//     !isMobSkillState && hasPreventActionEffect
//
// Dual-wire of Go aicontainer.TickPreventActionParkAllowed
// (tick_prevent_action_park.go). Call site: CAIContainer::Tick post-drain
// prevent-action park. Inactive construction remains host-only.
// Sibling dual-wires left alone: TickStateLoopContinue /
// InternalEngageShouldResumeInactive free functions.
inline auto TickPreventActionParkAllowed(
    const bool hasBattleEntity,
    const bool isAlive,
    const bool isInactiveState,
    const bool isMagicState,
    const bool isMobSkillState,
    const bool hasPreventActionEffect) -> bool
{
    return hasBattleEntity && isAlive && !isInactiveState && !isMagicState && !isMobSkillState && hasPreventActionEffect;
}

// ShouldInterruptCurrent reports whether InterruptStates should cleanup and
// resume past the current state for one loop iteration.
// Mirrors:
//   while (m_currentState && m_currentState->CanInterrupt()) { Cleanup; resume; }
// Formula (slice 6321):
//   hasCurrentState && currentCanInterrupt
// Dual-wire of Go aicontainer.ShouldInterruptCurrent (interrupt_states.go).
// Call site: CAIContainer::InterruptStates while admission.
// Cleanup/resumeNextState object graph remains host/deferred.
inline auto ShouldInterruptCurrent(const bool hasCurrentState, const bool currentCanInterrupt) -> bool
{
    return hasCurrentState && currentCanInterrupt;
}

// ShouldClearCurrentState reports whether ClearStateStack should cleanup and
// resume past the current state for one loop iteration.
// Mirrors: while (m_currentState) { Cleanup; resumeNextState; }
// Formula (slice 6322): hasCurrentState
// Dual-wire of Go aicontainer.ShouldClearCurrentState (state_stack_cleanup.go).
// Call site: CAIContainer::ClearStateStack while admission.
inline auto ShouldClearCurrentState(const bool hasCurrentState) -> bool
{
    return hasCurrentState;
}

// ShouldCleanupCompletedState reports whether CheckCompletedStates should
// cleanup and resume past the current state for one loop iteration.
// Mirrors: while (m_currentState && m_currentState->IsCompleted()) { Cleanup; resume; }
// Formula (slice 6322): hasCurrentState && isCompleted
// Dual-wire of Go aicontainer.ShouldCleanupCompletedState (state_stack_cleanup.go).
// Call site: CAIContainer::CheckCompletedStates while admission.
inline auto ShouldCleanupCompletedState(const bool hasCurrentState, const bool isCompleted) -> bool
{
    return hasCurrentState && isCompleted;
}

// IsActionQueueEmpty reports whether CAIContainer::QueueEmpty should return true.
// Mirrors: return ActionQueue.isEmpty();
// Formula (slice 6323): isEmpty
// Dual-wire of Go aicontainer.IsActionQueueEmpty (queue_empty.go).
// Call site: CAIContainer::QueueEmpty. ActionQueue ownership remains host.
inline auto IsActionQueueEmpty(const bool isEmpty) -> bool
{
    return isEmpty;
}

// ShouldSuspendCurrentOnEnter reports whether enterState should push the
// existing current state onto the stack before installing next.
// Mirrors: if (m_currentState) { m_stateStack.push(std::move(m_currentState)); }
// Formula (slice 6327): hasCurrentState
// Dual-wire of Go aicontainer.ShouldSuspendCurrentOnEnter (enter_resume_state.go).
// Call site: CAIContainer::enterState. unique_ptr moves remain host.
inline auto ShouldSuspendCurrentOnEnter(const bool hasCurrentState) -> bool
{
    return hasCurrentState;
}

// ShouldResumeStackedState reports whether resumeNextState should pop and
// install the suspended state (true) versus clear current (false when empty).
// Mirrors: if (m_stateStack.empty()) { reset } else { pop install }
// Formula (slice 6327): !stackEmpty
// Dual-wire of Go aicontainer.ShouldResumeStackedState (enter_resume_state.go).
// Call site: CAIContainer::resumeNextState. unique_ptr moves remain host.
inline auto ShouldResumeStackedState(const bool stackEmpty) -> bool
{
    return !stackEmpty;
}

// ShouldTickFollowPath reports whether Tick may drive PathFind::FollowPath.
// Mirrors: if (!Controller && CanFollowPath() && !isPathingPaused)
// Formula (slice 6359): !hasController && canFollowPath && !pathingPaused
// Dual-wire of Go aicontainer.ShouldTickFollowPath (tick_pathing.go).
// Call site: CAIContainer::Tick pathing block. CanFollowPath is precomputed.
// FollowPath / OnPath Lua remain host.
inline auto ShouldTickFollowPath(const bool hasController, const bool canFollowPath, const bool pathingPaused) -> bool
{
    return !hasController && canFollowPath && !pathingPaused;
}

// ShouldNotifyPathPoint reports whether Tick should fire PATH / OnPath.
// Mirrors: if (PathFind->OnPoint()) { EventHandler...; luautils::OnPath(...); }
// Formula (slice 6359): onPoint
// Dual-wire of Go aicontainer.ShouldNotifyPathPoint (tick_pathing.go).
// Call site: CAIContainer::Tick after FollowPath. Listener/Lua remain host.
inline auto ShouldNotifyPathPoint(const bool onPoint) -> bool
{
    return onPoint;
}

// ShouldClearPathOnReset reports whether Reset should Clear PathFind.
// Mirrors: if (PathFind) { PathFind->Clear(); }
// Formula (slice 6360): hasPathFind
// Dual-wire of Go aicontainer.ShouldClearPathOnReset (reset_trigger_pathing.go).
// Call site: CAIContainer::Reset. Clear body remains host PathFind ownership.
inline auto ShouldClearPathOnReset(const bool hasPathFind) -> bool
{
    return hasPathFind;
}

// ShouldResetControllerOnReset reports whether Reset should call Controller->Reset.
// Mirrors: if (Controller) { Controller->Reset(); }
// Formula (slice 6360): hasController
// Dual-wire of Go aicontainer.ShouldResetControllerOnReset (reset_trigger_pathing.go).
// Call site: CAIContainer::Reset.
inline auto ShouldResetControllerOnReset(const bool hasController) -> bool
{
    return hasController;
}

// ShouldPausePathingOnTrigger reports whether Trigger should set pauseNPCPathing.
// Mirrors: if (PathFind && GetLocalVar("stopPathingOnTrigger") == 1)
// Formula (slice 6360): hasPathFind && stopPathingOnTrigger == 1
// Dual-wire of Go aicontainer.ShouldPausePathingOnTrigger (reset_trigger_pathing.go).
// Call site: CAIContainer::Trigger after ChangeState when CanChangeState.
// SetLocalVar remains host-owned.
inline auto ShouldPausePathingOnTrigger(const bool hasPathFind, const uint32_t stopPathingOnTrigger) -> bool
{
    return hasPathFind && stopPathingOnTrigger == 1;
}

} // namespace aicontainerhelpers
