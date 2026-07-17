#pragma once

// Pure AttackTarget / RetreatToMaster gates from petutils.
// Parity: internal/petutils/engage.go (slice 1627 residual suite);
// internal/petutils/pet_engage.go (slice 3051 dual-wire ShouldPetEngage).
//
// Dual-wire index:
//   - 3051: ShouldPetEngage (!hasPreventActionEffect on AttackTarget)
//
// Production host: petutils::AttackTarget (petutils.cpp) injects master/pet/
// target non-null into CanAttackTarget; on fail ShowWarning + return.
// Then injects PPet->StatusEffectContainer->HasPreventActionEffect() into
// ShouldPetEngage; on true PPet->PAI->Engage(PTarget->targid).
// Go dual-wire: petutils.ShouldPetEngage (internal/petutils/pet_engage.go).
// Production host: petutils::RetreatToMaster (petutils.cpp) injects master/pet
// non-null into CanRetreatToMaster; on fail ShowWarning + return.
// Then injects HasPreventActionEffect() into ShouldPetDisengage; on true
// PPet->PAI->Disengage(). Sibling residual: CanAttackTarget, CanRetreatToMaster,
// ShouldPetDisengage remain on 1627 residual surface (not dual-wired in 3051).

namespace petengagehelpers
{

// CanAttackTarget is the null preflight for AttackTarget (master, pet, target).
// Residual pure port (slice 1627); not dual-wired in 3051.
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
// test_pet_should_engage_3051. Sibling residual gates: CanAttackTarget,
// CanRetreatToMaster, ShouldPetDisengage (not dual-wired here).
inline auto ShouldPetEngage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// CanRetreatToMaster is the null preflight for RetreatToMaster.
// Residual pure port (slice 1627); not dual-wired in 3051.
inline auto CanRetreatToMaster(const bool hasMaster, const bool hasPet) -> bool
{
    return hasMaster && hasPet;
}

// ShouldPetDisengage is true when the pet is not under a prevent-action effect.
// Residual pure port (slice 1627); not dual-wired in 3051.
inline auto ShouldPetDisengage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

} // namespace petengagehelpers
