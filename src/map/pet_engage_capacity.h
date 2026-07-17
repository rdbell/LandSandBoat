#pragma once

// Pure AttackTarget / RetreatToMaster gates from petutils.
// Parity: internal/petutils/engage.go (slice 1627 residual suite);
// internal/petutils/pet_engage.go (slice 3536 dedicated dual-wire
// ShouldPetEngage; residual expand 3051 / prior dedicated 3484 / 3429 / 3375 / pure 1627);
// internal/petutils/can_attack_target.go (slice 3328 dedicated dual-wire
// CanAttackTarget; residual expand 3071 / prior dedicated 3298 / 3269 / pure 1627);
// internal/petutils/pet_disengage.go (slice 3081 dual-wire ShouldPetDisengage);
// internal/petutils/can_retreat_master.go (slice 3097 dual-wire CanRetreatToMaster).
//
// Dual-wire index:
//   - 3051: ShouldPetEngage residual dual-wire suite
//           (!hasPreventActionEffect on AttackTarget)
//   - 3071: CanAttackTarget residual dual-wire suite
//           (hasMaster && hasPet && hasTarget null preflight)
//   - 3081: ShouldPetDisengage (!hasPreventActionEffect on RetreatToMaster)
//   - 3097: CanRetreatToMaster (hasMaster && hasPet null preflight)
//   - 3269: CanAttackTarget prior dedicated dual-wire expand residual 3071
//           (can_attack_target.go; retained)
//   - 3298: CanAttackTarget prior dedicated dual-wire expand residual 3071
//           (can_attack_target.go; retained; prior ~3269)
//   - 3328: CanAttackTarget dedicated dual-wire
//           (can_attack_target.go; expand residual 3071; prior 3298 / 3269; pure 1627)
//   - 3375: ShouldPetEngage prior dedicated dual-wire expand residual 3051
//           (pet_engage.go; retained; pure 1627)
//   - 3429: ShouldPetEngage prior dedicated dual-wire expand residual 3051
//           (pet_engage.go; retained; prior dedicated 3375; pure 1627)
//   - 3484: ShouldPetEngage prior dedicated dual-wire expand residual 3051
//           (pet_engage.go; retained; prior dedicated 3429 / 3375; pure 1627)
//   - 3536: ShouldPetEngage dedicated dual-wire
//           (pet_engage.go; expand residual 3051; prior dedicated 3484 / 3429 / 3375; pure 1627)
//
// Production host: petutils::AttackTarget (petutils.cpp) injects master/pet/
// target non-null into CanAttackTarget; on fail ShowWarning + return.
// Then injects PPet->StatusEffectContainer->HasPreventActionEffect() into
// ShouldPetEngage; on true PPet->PAI->Engage(PTarget->targid).
// Go dual-wire: petutils.CanAttackTarget (internal/petutils/can_attack_target.go);
// petutils.ShouldPetEngage (internal/petutils/pet_engage.go).
// Residual dual-wire suite (ShouldPetEngage): 3051 (test_pet_should_engage_3051).
// Prior dedicated dual-wire suites (ShouldPetEngage): 3375 (test_petutils_pet_engage_3375; retained),
// 3429 (test_petutils_pet_engage_3429; retained), 3484 (test_petutils_pet_engage_3484; retained).
// Dedicated dual-wire suite (ShouldPetEngage): 3536 (test_petutils_pet_engage_3536).
// Residual dual-wire suite (CanAttackTarget): 3071 (test_pet_can_attack_target_3071).
// Prior dedicated dual-wire suites: 3269 (test_petutils_can_attack_target_3269;
// retained), 3298 (test_petutils_can_attack_target_3298; retained).
// Dedicated dual-wire suite (CanAttackTarget): 3328 (test_petutils_can_attack_target_3328).
// Production host: petutils::RetreatToMaster (petutils.cpp) injects master/pet
// non-null into CanRetreatToMaster; on fail ShowWarning + return.
// Then injects HasPreventActionEffect() into ShouldPetDisengage; on true
// PPet->PAI->Disengage(). Go dual-wire: petutils.CanRetreatToMaster
// (internal/petutils/can_retreat_master.go); petutils.ShouldPetDisengage
// (internal/petutils/pet_disengage.go).

namespace petengagehelpers
{

// ---------------------------------------------------------------------------
// Slice 3328 — AttackTarget null preflight (dedicated expand residual 3071;
// prior dedicated 3298 / 3269 retained)
// ---------------------------------------------------------------------------

// CanAttackTarget is the null preflight for AttackTarget (master, pet, target).
//
// Formula (slice 3328 dedicated dual-wire expand residual 3071; prior dedicated
// 3298 / 3269 / residual 3071 / pure 1627 — formula unchanged):
//   hasMaster && hasPet && hasTarget
//
// hasMaster  — host-evaluated PMaster != nullptr
// hasPet     — host-evaluated PMaster->PPet != nullptr
// hasTarget  — host-evaluated PTarget != nullptr
// true  → admit AttackTarget path (proceed to ShouldPetEngage)
// false → ShowWarning + return (null master/pet/target)
//
// Null preflight: all three presence injects must be true before the
// prevent-action engage gate.
//
// Dual-wire of Go petutils.CanAttackTarget
// (internal/petutils/can_attack_target.go).
// Call site: petutils::AttackTarget — host injects PMaster / PMaster->PPet /
// PTarget non-null; on false ShowWarning + return. On true proceeds to
// ShouldPetEngage (slice 3536 dedicated dual-wire; prior 3484 / 3429 / 3375 / residual 3051).
// Prior pure port: slice 1627 (AttackTarget / RetreatToMaster pure gates).
// Residual dual-wire suite: 3071 / test_pet_can_attack_target_3071.
// Prior dedicated dual-wire suites: 3269 / test_petutils_can_attack_target_3269
// (retained), 3298 / test_petutils_can_attack_target_3298 (retained).
// Dedicated dual-wire suite is test_petutils_can_attack_target_3328.
// Residual pins remain in test_pet_engage_1627. Sibling dual-wire:
// ShouldPetEngage (3536 / prior 3484 / 3429 / 3375 / residual 3051), ShouldPetDisengage (3081),
// CanRetreatToMaster (3097).
inline auto CanAttackTarget(const bool hasMaster, const bool hasPet, const bool hasTarget) -> bool
{
    return hasMaster && hasPet && hasTarget;
}

// ---------------------------------------------------------------------------
// Slice 3536 — AttackTarget prevent-action engage gate (dedicated expand
// residual 3051; prior dedicated 3484 / 3429 / 3375 retained; pure 1627 retained)
// ---------------------------------------------------------------------------

// ShouldPetEngage mirrors !HasPreventActionEffect before pet PAI Engage.
//
// Formula (slice 3536 dedicated dual-wire expand residual 3051; prior dedicated
// 3484 / 3429 / 3375 / pure 1627 — formula unchanged):
//   !hasPreventActionEffect
//
// hasPreventActionEffect — host-evaluated pet StatusEffectContainer
//                          HasPreventActionEffect()
// true  → pet may engage battle target (PAI->Engage)
// false → skip engage (prevent-action effect active)
//
// Polarity gate: engage only when the pet is not under a prevent-action status.
//
// Dual-wire of Go petutils.ShouldPetEngage
// (internal/petutils/pet_engage.go).
// Call site: petutils::AttackTarget — host injects
// PPet->StatusEffectContainer->HasPreventActionEffect() after CanAttackTarget.
// Prior pure port: slice 1627 (AttackTarget / RetreatToMaster pure gates).
// Residual dual-wire suite: 3051 / test_pet_should_engage_3051.
// Prior dedicated dual-wire suites: 3375 / test_petutils_pet_engage_3375 (retained),
// 3429 / test_petutils_pet_engage_3429 (retained),
// 3484 / test_petutils_pet_engage_3484 (retained).
// Dedicated dual-wire suite is test_petutils_pet_engage_3536.
// Residual pins remain in test_pet_engage_1627. Sibling dual-wire:
// CanAttackTarget (3328 / prior 3298 / 3269 / residual 3071),
// ShouldPetDisengage (3081), CanRetreatToMaster (3097).
inline auto ShouldPetEngage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// CanRetreatToMaster is the null preflight for RetreatToMaster (master, pet).
//
// Formula (slice 3097 dual-wire):
//   hasMaster && hasPet
//
// hasMaster — host-evaluated PMaster != nullptr
// hasPet    — host-evaluated PMaster->PPet != nullptr
// true  → admit RetreatToMaster path (proceed to ShouldPetDisengage)
// false → ShowWarning + return (null master/pet)
//
// Null preflight: both presence injects must be true before the
// prevent-action disengage gate.
//
// Dual-wire of Go petutils.CanRetreatToMaster.
// Call site: petutils::RetreatToMaster — host injects PMaster / PMaster->PPet
// non-null; on false ShowWarning + return. On true proceeds to
// ShouldPetDisengage (slice 3081 dual-wire).
// Prior pure port: slice 1627 (AttackTarget / RetreatToMaster pure gates).
// Residual pins remain in test_pet_engage_1627; dedicated dual-wire suite is
// test_pet_can_retreat_master_3097. Sibling dual-wire: ShouldPetEngage (3536 /
// prior 3484 / 3429 / 3375 / residual 3051), CanAttackTarget (3328 / prior 3298 / 3269 /
// residual 3071), ShouldPetDisengage (3081).
inline auto CanRetreatToMaster(const bool hasMaster, const bool hasPet) -> bool
{
    return hasMaster && hasPet;
}

// ShouldPetDisengage is true when the pet is not under a prevent-action effect.
//
// Formula (slice 3081 dual-wire):
//   !hasPreventActionEffect
//
// hasPreventActionEffect — host-evaluated pet StatusEffectContainer
//                          HasPreventActionEffect()
// true  → pet may disengage (PAI->Disengage)
// false → skip disengage (prevent-action effect active)
//
// Polarity gate: disengage only when the pet is not under a prevent-action
// status.
//
// Dual-wire of Go petutils.ShouldPetDisengage.
// Call site: petutils::RetreatToMaster — host injects
// PPet->StatusEffectContainer->HasPreventActionEffect() after CanRetreatToMaster.
// Prior pure port: slice 1627 (AttackTarget / RetreatToMaster pure gates).
// Residual pins remain in test_pet_engage_1627; dedicated dual-wire suite is
// test_pet_should_disengage_3081. Sibling dual-wire: ShouldPetEngage (3536 /
// prior 3484 / 3429 / 3375 / residual 3051), CanAttackTarget (3328 / prior 3298 / 3269 /
// residual 3071), CanRetreatToMaster (3097).
inline auto ShouldPetDisengage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

} // namespace petengagehelpers
