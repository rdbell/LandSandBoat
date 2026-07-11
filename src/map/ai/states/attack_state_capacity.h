#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CAttackState policy halves (Update exit/ready/disengage gates, CanAttack
// success → delay add, auto-target candidate filters). Entity CanAttack,
// OnAttack, path clear, and packets remain host-injected.

namespace attackstatehelpers
{

// ANIMATION_ATTACK pin (base_entity.h).
constexpr uint8 AnimationAttack = 1;

// Auto-target distance band from UpdateTarget PC spawn scan.
constexpr float AutoTargetMaxDistance = 10.0f;

// Facing cone used by auto-target (facing(..., 64)).
constexpr uint8 AutoTargetFacingCone = 64;

// ActionCategory::None pin.
constexpr uint8 ActionCategoryNone = 0;

// ShouldExitNoTarget mirrors !PTarget || PTarget->isDead().
inline auto ShouldExitNoTarget(const bool hasTarget, const bool targetDead) -> bool
{
    return !hasTarget || targetDead;
}

// ShouldDisengageAfterCanAttack mirrors GetBattleTargetID() == 0 after CanAttack.
inline auto ShouldDisengageBattleTargetZero(const bool battleTargetIDZero) -> bool
{
    return battleTargetIDZero;
}

// CanAttackSucceeded mirrors ret && !m_errorMsg after entity CanAttack.
inline auto CanAttackSucceeded(const bool canAttackRet, const bool hasErrorMsg) -> bool
{
    return canAttackRet && !hasErrorMsg;
}

// ShouldAddWeaponDelayAfterCanAttack mirrors CanAttackSucceeded for timer add.
inline auto ShouldAddWeaponDelayAfterCanAttack(const bool canAttackRet, const bool hasErrorMsg) -> bool
{
    return CanAttackSucceeded(canAttackRet, hasErrorMsg);
}

// ShouldSendAttackPacket mirrors action.actiontype != ActionCategory::None.
// Host injects the action category ordinal.
inline auto ShouldSendAttackPacket(const uint8 actionCategory) -> bool
{
    return actionCategory != ActionCategoryNone;
}

// ShouldHandleAttackError mirrors !CanAttack && OnAttackError returned true.
// Host injects both flags.
inline auto ShouldHandleAttackError(const bool canAttackSucceeded, const bool onAttackError) -> bool
{
    return !canAttackSucceeded && onAttackError;
}

// ShouldSubtractAttackElapsed mirrors !AttackReady branch.
inline auto ShouldSubtractAttackElapsed(const bool attackReady) -> bool
{
    return !attackReady;
}

// ShouldCleanupDisengage mirrors !isDead on Cleanup.
inline auto ShouldCleanupDisengage(const bool isDead) -> bool
{
    return !isDead;
}

// ShouldClearPathOnEnter mirrors PEntity->PAI->PathFind truthy (host injects).
inline auto ShouldClearPathOnEnter(const bool hasPathFind) -> bool
{
    return hasPathFind;
}

// ShouldThrowInitNoTarget mirrors !GetTarget() || m_errorMsg in ctor.
inline auto ShouldThrowInitNoTarget(const bool hasTarget, const bool hasErrorMsg) -> bool
{
    return !hasTarget || hasErrorMsg;
}

// AutoTargetCandidate mirrors PC spawn scan filters:
//   animation == ATTACK && facing && distance <= 10
// Host injects facing/distance results.
inline auto AutoTargetCandidate(
    const uint8 animation,
    const bool isFacing,
    const float distance) -> bool
{
    return animation == AnimationAttack && isFacing && distance <= AutoTargetMaxDistance;
}

// ShouldTryAutoTarget mirrors PC && hasAutoTargetEnabled when invalid battle target.
inline auto ShouldTryAutoTarget(const bool isPC, const bool autoTargetEnabled) -> bool
{
    return isPC && autoTargetEnabled;
}

// ShouldChangeTarget mirrors targid != newTargid in UpdateTarget.
inline auto ShouldChangeTarget(const uint16 currentTargid, const uint16 newTargid) -> bool
{
    return currentTargid != newTargid;
}

// ShouldNotifyChangeTarget mirrors ShouldChangeTarget && currentTargid != 0.
inline auto ShouldNotifyChangeTarget(const uint16 currentTargid, const uint16 newTargid) -> bool
{
    return currentTargid != newTargid && currentTargid != 0;
}

// ShouldClearErrorOnMissingNewTarget mirrors !PNewTarget after change notify.
inline auto ShouldClearErrorOnMissingNewTarget(const bool hasNewTarget) -> bool
{
    return !hasNewTarget;
}

// ShouldResolveBattleTarget mirrors newTargid != 0 before IsValidTarget.
inline auto ShouldResolveBattleTarget(const uint16 battleTargetID) -> bool
{
    return battleTargetID != 0;
}

// AttackUpdateOutcome is the pure Update top-level decision (before host OnAttack).
enum class AttackUpdateOutcome : uint8
{
    Continue         = 0, // not ready: subtract elapsed
    ExitNoTarget     = 1, // no/dead target
    ExitDisengaged   = 2, // battle target cleared
    PerformAttack    = 3, // ready + CanAttack success + target still set
    HandleError      = 4, // ready + CanAttack fail + OnAttackError
    ReadyNoAction    = 5, // ready + fail + no OnAttackError, still engaged
};

// ResolveAttackUpdate is the pure decision ladder of CAttackState::Update
// through the CanAttack / disengage checks (host supplies inject flags).
inline auto ResolveAttackUpdate(
    const bool hasTarget,
    const bool targetDead,
    const bool attackReady,
    const bool canAttackSucceeded,
    const bool battleTargetZeroAfterCanAttack,
    const bool onAttackError,
    const bool battleTargetZeroAfterError) -> AttackUpdateOutcome
{
    if (ShouldExitNoTarget(hasTarget, targetDead))
    {
        return AttackUpdateOutcome::ExitNoTarget;
    }
    if (!attackReady)
    {
        return AttackUpdateOutcome::Continue;
    }
    if (canAttackSucceeded)
    {
        if (battleTargetZeroAfterCanAttack)
        {
            return AttackUpdateOutcome::ExitDisengaged;
        }
        return AttackUpdateOutcome::PerformAttack;
    }
    // CanAttack failed
    if (onAttackError)
    {
        if (battleTargetZeroAfterError)
        {
            // After error handling, still check battle target zero.
            // Original: HandleError then check GetBattleTargetID()==0.
            return AttackUpdateOutcome::HandleError; // host then may exit
        }
        return AttackUpdateOutcome::HandleError;
    }
    if (battleTargetZeroAfterError)
    {
        return AttackUpdateOutcome::ExitDisengaged;
    }
    return AttackUpdateOutcome::ReadyNoAction;
}

// ShouldExitAfterReadyPath mirrors GetBattleTargetID()==0 after ready branch.
inline auto ShouldExitAfterReadyPath(const bool battleTargetZero) -> bool
{
    return battleTargetZero;
}

} // namespace attackstatehelpers
