#pragma once

// Pure CAIContainer controller-dispatch gates extracted so native tests can
// pin policy without entity/controller instances, state objects, or packets.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2947: CanDispatch residual dual-wire expand
//           (controller presence outer gate for public
//           Cast/Engage/ChangeTarget/Disengage/WeaponSkill/Ability/
//           RangedAttack and typed MobSkill/PetSkill/UseItem)
//   - 2952: CanChangeState (current-state change gate for external means)
//   - 3222: CanDispatch dedicated dual-wire
//           (hasController identity; residual expand 2947 / pure 1189)
//
// Production host: CAIContainer::{Cast,Engage,...} (ai_container.cpp) inject
// Controller / typed dynamic_cast presence into CanDispatch before invoking
// the controller method. CAIContainer::CanChangeState injects current-state
// presence and current->CanChangeState() into CanChangeState.
// Go dual-wire: aicontainer.CanDispatch (can_dispatch.go),
// aicontainer.CanChangeState (can_change_state.go). Prior pure port: slice 1189.

namespace aicontainerhelpers
{

// CanDispatch reports whether a controller is present to forward into.
// Mirrors the outer gate of CAIContainer::{Cast,Engage,...}:
//
//   if (Controller) { return Controller->X(...); }
//   return false;
//
// Formula (slice 3222 dedicated dual-wire; residual expand 2947 / pure 1189 —
// formula unchanged):
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
// test_aicontainer_can_dispatch_2947. Dedicated dual-wire suite is
// test_aicontainer_can_dispatch_3222. Formula is unchanged; this slice
// only expands dual-wire docs + index + dedicated suite.
// Sibling dual-wire left alone: 2952 CanChangeState.
inline auto CanDispatch(const bool hasController) -> bool
{
    return hasController;
}

// CanChangeState reports whether the AI may change state from external means.
// Mirrors CAIContainer::CanChangeState:
//
//   return !GetCurrentState() || GetCurrentState()->CanChangeState();
//
// Formula (slice 2952 dual-wire):
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
inline auto CanChangeState(const bool hasCurrentState, const bool currentCanChange) -> bool
{
    return !hasCurrentState || currentCanChange;
}

} // namespace aicontainerhelpers
