#pragma once

// Pure AttackTarget / RetreatToMaster gates from petutils.
// Parity: internal/petutils/engage.go (slice 1627 residual suite);
// internal/petutils/pet_engage.go (slice 3051 dual-wire ShouldPetEngage);
// internal/petutils/can_attack_target.go (slice 3071 dual-wire CanAttackTarget);
// internal/petutils/pet_disengage.go (slice 3081 dual-wire ShouldPetDisengage).
//
// Dual-wire index:
//   - 3051: ShouldPetEngage (!hasPreventActionEffect on AttackTarget)
//   - 3071: CanAttackTarget (hasMaster && hasPet && hasTarget null preflight)
//   - 3081: ShouldPetDisengage (!hasPreventActionEffect on RetreatToMaster)
//
// Production host: petutils::AttackTarget (petutils.cpp) injects master/pet/
// target non-null into CanAttackTarget; on fail ShowWarning + return.
// Then injects PPet->StatusEffectContainer->HasPreventActionEffect() into
// ShouldPetEngage; on true PPet->PAI->Engage(PTarget->targid).
// Go dual-wire: petutils.CanAttackTarget (internal/petutils/can_attack_target.go);
// petutils.ShouldPetEngage (internal/petutils/pet_engage.go).
// Production host: petutils::RetreatToMaster (petutils.cpp) injects master/pet
// non-null into CanRetreatToMaster; on fail ShowWarning + return.
// Then injects HasPreventActionEffect() into ShouldPetDisengage; on true
// PPet->PAI->Disengage(). Go dual-wire: petutils.ShouldPetDisengage
// (internal/petutils/pet_disengage.go). Sibling residual: CanRetreatToMaster
// remains on 1627 residual surface (not dual-wired in 3051/3071/3081).

namespace petengagehelpers
{

// CanAttackTarget is the null preflight for AttackTarget (master, pet, target).
//
// Formula (slice 3071 dual-wire):
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
// Dual-wire of Go petutils.CanAttackTarget.
// Call site: petutils::AttackTarget — host injects PMaster / PMaster->PPet /
// PTarget non-null; on false ShowWarning + return. On true proceeds to
// ShouldPetEngage (slice 3051 dual-wire).
// Prior pure port: slice 1627 (AttackTarget / RetreatToMaster pure gates).
// Residual pins remain in test_pet_engage_1627; dedicated dual-wire suite is
// test_pet_can_attack_target_3071. Sibling dual-wire: ShouldPetEngage (3051),
// ShouldPetDisengage (3081). Sibling residual gate: CanRetreatToMaster (not
// dual-wired here).
inline auto CanAttackTarget(const bool hasMaster, const bool hasPet, const bool hasTarget) -> bool
{
    return hasMaster && hasPet && hasTarget;
}

// ShouldPetEngage mirrors !HasPreventActionEffect before pet PAI Engage.
//
// Formula (slice 3051 dual-wire):
//   !hasPreventActionEffect
//
// hasPreventActionEffect — host-evaluated pet StatusEffectContainer
//                          HasPreventActionEffect()
// true  → pet may engage battle target (PAI->Engage)
// false → skip engage (prevent-action effect active)
//
// Polarity gate: engage only when the pet is not under a prevent-action status.
//
// Dual-wire of Go petutils.ShouldPetEngage.
// Call site: petutils::AttackTarget — host injects
// PPet->StatusEffectContainer->HasPreventActionEffect() after CanAttackTarget.
// Prior pure port: slice 1627 (AttackTarget / RetreatToMaster pure gates).
// Residual pins remain in test_pet_engage_1627; dedicated dual-wire suite is
// test_pet_should_engage_3051. Sibling dual-wire: CanAttackTarget (3071),
// ShouldPetDisengage (3081). Sibling residual gate: CanRetreatToMaster (not
// dual-wired here).
inline auto ShouldPetEngage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// CanRetreatToMaster is the null preflight for RetreatToMaster.
// Residual pure port (slice 1627); not dual-wired in 3051/3071/3081.
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
// test_pet_should_disengage_3081. Sibling dual-wire: ShouldPetEngage (3051),
// CanAttackTarget (3071). Sibling residual gate: CanRetreatToMaster (not
// dual-wired here).
inline auto ShouldPetDisengage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

} // namespace petengagehelpers
