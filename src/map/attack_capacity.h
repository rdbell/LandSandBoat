#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cstdint>

// Pure CAttack check-policy halves for parry/guard/deflect/counter/cover.

namespace attackhelpers
{

// PHYSICAL_ATTACK_TYPE::DAKEN numeric.
constexpr uint8 AttackTypeDaken = 9;

// Counter rate hard cap used with COUNTER mod + merit.
constexpr uint16 CounterRateHardCap = 80;

// Seigan counter is Zanshin rate / 4 after clamp.
constexpr uint16 SeiganCounterDivisor = 4;

// Facing arc used for counter (64 units of 256).
constexpr uint8 CounterFacingArc = 64;

// IsDakenAttack mirrors attack type == DAKEN exclusions.
inline auto IsDakenAttack(const uint8 attackType) -> bool
{
    return attackType == AttackTypeDaken;
}

// ShouldSkipParryForDaken mirrors CheckParried Daken early-out.
inline auto ShouldSkipParryForDaken(const uint8 attackType) -> bool
{
    return IsDakenAttack(attackType);
}

// ShouldSkipCounterForDaken mirrors CheckCounter Daken early-out.
inline auto ShouldSkipCounterForDaken(const uint8 attackType) -> bool
{
    return IsDakenAttack(attackType);
}

// ShouldSkipAnticipateForDaken mirrors CheckAnticipated Daken early-out.
inline auto ShouldSkipAnticipateForDaken(const uint8 attackType) -> bool
{
    return IsDakenAttack(attackType);
}

// ApplyGuardDamageRatio mirrors damageRatio -= 1.0f clamped to >= 0.
inline auto ApplyGuardDamageRatio(const float damageRatio) -> float
{
    return std::max(damageRatio - 1.0f, 0.0f);
}

// IsDeflected mirrors DefenseBoost present + subpower > 0 + infront(host).
// hasDefenseBoost and subpower injected; facing result injected as inFront.
inline auto IsDeflected(const bool hasDefenseBoost, const uint16 subPower, const bool inFront) -> bool
{
    if (!hasDefenseBoost || subPower == 0)
    {
        return false;
    }
    return inFront;
}

// ShouldBlockCounterForState mirrors !engaged || HasPreventActionEffect(true).
inline auto ShouldBlockCounterForState(const bool isEngaged, const bool hasPreventActionIgnoringCharm) -> bool
{
    return !isEngaged || hasPreventActionIgnoringCharm;
}

// ShouldAddMNKCounterMerit mirrors TYPE_PC && MNK main.
inline auto ShouldAddMNKCounterMerit(const bool isPC, const bool isMNKMain) -> bool
{
    return isPC && isMNKMain;
}

// ClampCounterRate mirrors clamp(COUNTER + merit, 0, 80).
inline auto ClampCounterRate(const int16 counterMod, const uint8 meritCounter) -> uint16
{
    int32 total = static_cast<int32>(counterMod) + static_cast<int32>(meritCounter);
    if (total < 0)
    {
        return 0;
    }
    if (total > static_cast<int32>(CounterRateHardCap))
    {
        return CounterRateHardCap;
    }
    return static_cast<uint16>(total);
}

// ComputeSeiganCounterChance mirrors (clamp Zanshin+merit, 0..100) / 4 when valid seigan.
inline auto ComputeSeiganCounterChance(
    const bool hasValidSeigan,
    const uint16 zanshinMod,
    const uint16 zanshinMerit) -> uint16
{
    if (!hasValidSeigan)
    {
        return 0;
    }
    uint16 chance = zanshinMod + zanshinMerit;
    if (chance > 100)
    {
        chance = 100;
    }
    return static_cast<uint16>(chance / SeiganCounterDivisor);
}

// IsValidSeiganForCounter mirrors 2H weapon && Seigan status.
inline auto IsValidSeiganForCounter(const bool isTwoHandedWeapon, const bool hasSeigan) -> bool
{
    return isTwoHandedWeapon && hasSeigan;
}

// ShouldSkipCounterForPerfectDodge mirrors attacker has Perfect Dodge.
inline auto ShouldSkipCounterForPerfectDodge(const bool attackerHasPerfectDodge) -> bool
{
    return attackerHasPerfectDodge;
}

// ShouldAttemptCounterRate mirrors rate roll OR seigan roll (host injects both procs).
inline auto ShouldAttemptCounterRate(const bool counterRateProcs, const bool seiganRateProcs) -> bool
{
    return counterRateProcs || seiganRateProcs;
}

// ShouldLandCounter mirrors rate attempt && facing && hit-rate proc.
inline auto ShouldLandCounter(const bool rateAttempt, const bool facing, const bool hitRateProcs) -> bool
{
    return rateAttempt && facing && hitRateProcs;
}

// ShouldPerfectCounter mirrors Perfect Counter status after normal counter miss path.
// Host only evaluates this when rate/facing path did not land.
inline auto ShouldPerfectCounter(const bool hasPerfectCounter) -> bool
{
    return hasPerfectCounter;
}

// IsCoverActive mirrors cover ability user non-null && alive.
inline auto IsCoverActive(const bool coverUserPresent, const bool coverUserAlive) -> bool
{
    return coverUserPresent && coverUserAlive;
}

// HasThirdEyeForAnticipate mirrors third eye effect present.
inline auto HasThirdEyeForAnticipate(const bool hasThirdEye) -> bool
{
    return hasThirdEye;
}

// ClampZanshinRate mirrors clamp for seigan path input before /4.
inline auto ClampZanshinRate(const uint16 rate) -> uint16
{
    return rate > 100 ? static_cast<uint16>(100) : rate;
}

} // namespace attackhelpers
