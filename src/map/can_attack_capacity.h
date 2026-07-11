#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CanAttack / melee-range policy halves from CBattleEntity, CCharEntity,
// CMobEntity, and CPetEntity. Controller/PAI/claim lookups and packets remain
// host-injected.

namespace canattackhelpers
{

// Melee range pad from GetMeleeRange: casterHitbox + 2.0 + targetHitbox.
constexpr float MeleeRangePad = 2.0f;

// Char lose-sight distance when not charmed.
constexpr float CharLoseSightDistance = 30.0f;

// GetRangedAttackRange default.
constexpr float RangedAttackRangeDefault = 25.0f;

// Facing cone for char CanAttack (facing(..., 64)).
constexpr uint8 CharFacingCone = 64;

// MsgBasic pins for char/pet CanAttack failures.
enum class CanAttackFail : uint16
{
    None              = 0,
    AlreadyClaimed    = 12, // MsgBasic::AlreadyClaimed
    LoseSight         = 36, // MsgBasic::LoseSight
    UnableToSeeTarget = 5,  // MsgBasic::UnableToSeeTarget
    TargetOutOfRange  = 4,  // MsgBasic::TargetOutOfRange
    Untargetable      = 0xFFFE,
    AutoAttackOff     = 0xFFFD,
};

// GetMeleeRange mirrors modelHitboxSize + 2.0f + target->modelHitboxSize.
inline auto GetMeleeRange(const float casterHitbox, const float targetHitbox) -> float
{
    return casterHitbox + MeleeRangePad + targetHitbox;
}

// TooFarForMelee mirrors distance > meleeRange.
inline auto TooFarForMelee(const float distance, const float meleeRange) -> bool
{
    return distance > meleeRange;
}

// BattleCanAttack is the pure form of CBattleEntity::CanAttack:
//   !IsUntargetable && distance <= GetMeleeRange && autoAttackEnabled
// Returns true only when allowed (no error packet on base path).
inline auto BattleCanAttack(
    const bool untargetable,
    const float distance,
    const float meleeRange,
    const bool autoAttackEnabled) -> bool
{
    if (untargetable)
    {
        return false;
    }
    return !TooFarForMelee(distance, meleeRange) && autoAttackEnabled;
}

// MobSkillListAttackRange mirrors:
//   modelHitboxSize + skill->getDistance() + targetHitbox
// when MOBMOD_ATTACK_SKILL_LIST is set and skill list non-empty.
inline auto MobSkillListAttackRange(
    const float casterHitbox,
    const float skillDistance,
    const float targetHitbox) -> float
{
    return casterHitbox + skillDistance + targetHitbox;
}

// ShouldUseMobSkillListRange mirrors skill_list_id != 0.
inline auto ShouldUseMobSkillListRange(const int16 attackSkillListMod) -> bool
{
    return attackSkillListMod != 0;
}

// MobCanAttackWithSkillListRange mirrors the skill-list branch of CMobEntity::CanAttack
// (no untargetable check on that branch — original skill-list path skips it).
inline auto MobCanAttackWithSkillListRange(
    const float distance,
    const float attackRange,
    const bool autoAttackEnabled) -> bool
{
    return !TooFarForMelee(distance, attackRange) && autoAttackEnabled;
}

// CharCanAttack evaluates CCharEntity::CanAttack pure ladder.
// Host injects ownership, charm, facing, distances, melee range.
// On AlreadyClaimed / LoseSight, host should Disengage (side effect).
inline auto EvaluateCharCanAttack(
    const bool untargetable,
    const bool isMobOwner,
    const bool hasCharm,
    const float distance,
    const bool isFacing,
    const float meleeRange) -> CanAttackFail
{
    if (untargetable)
    {
        return CanAttackFail::Untargetable;
    }
    if (!isMobOwner)
    {
        return CanAttackFail::AlreadyClaimed;
    }
    if (!hasCharm && distance > CharLoseSightDistance)
    {
        return CanAttackFail::LoseSight;
    }
    if (!isFacing)
    {
        return CanAttackFail::UnableToSeeTarget;
    }
    if (TooFarForMelee(distance, meleeRange))
    {
        return CanAttackFail::TargetOutOfRange;
    }
    return CanAttackFail::None;
}

// ShouldDisengageOnCharCanAttackFail mirrors AlreadyClaimed / LoseSight paths.
inline auto ShouldDisengageOnCharCanAttackFail(const CanAttackFail fail) -> bool
{
    return fail == CanAttackFail::AlreadyClaimed || fail == CanAttackFail::LoseSight;
}

// PetCanAttackClaimFail mirrors pet master IsMobOwner gate before base CanAttack.
// Returns true when the pet should reject (AlreadyClaimed + Disengage).
inline auto PetCanAttackClaimFail(const bool hasPCMaster, const bool masterOwnsTarget) -> bool
{
    return hasPCMaster && !masterOwnsTarget;
}

// CharCanAttackSucceeded reports whether EvaluateCharCanAttack returned None.
inline auto CharCanAttackSucceeded(const CanAttackFail fail) -> bool
{
    return fail == CanAttackFail::None;
}

} // namespace canattackhelpers
