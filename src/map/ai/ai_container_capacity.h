#pragma once

// Pure CAIContainer controller-dispatch gates extracted so native tests can
// pin policy without entity/controller instances, state objects, or packets.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2947: CanDispatch (controller presence outer gate for public
//           Cast/Engage/ChangeTarget/Disengage/WeaponSkill/Ability/
//           RangedAttack and typed MobSkill/PetSkill/UseItem)
//
// Production host: CAIContainer::{Cast,Engage,...} (ai_container.cpp) inject
// Controller / typed dynamic_cast presence into CanDispatch before invoking
// the controller method.
// Go dual-wire: aicontainer.CanDispatch
// (internal/aicontainer/can_dispatch.go). Prior pure port: slice 1189.

namespace aicontainerhelpers
{

// CanDispatch reports whether a controller is present to forward into.
// Mirrors the outer gate of CAIContainer::{Cast,Engage,...}:
//
//   if (Controller) { return Controller->X(...); }
//   return false;
//
// Formula (slice 2947 dual-wire):
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
inline auto CanDispatch(const bool hasController) -> bool
{
    return hasController;
}

} // namespace aicontainerhelpers
