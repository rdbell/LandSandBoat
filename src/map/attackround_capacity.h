#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cstdint>

// Pure CAttackRound::CreateAttacks multi-hit preference and clamp policy.

namespace attackroundhelpers
{

// MAX_SWINGS hard cap used with Occasionally Attacks X Times gear.
constexpr uint8 MaxSwingsHardCap = 8;

// ClampMaxSwings mirrors std::min(mod, 8).
inline auto ClampMaxSwings(const uint8 maxSwingsMod) -> uint8
{
    return std::min(maxSwingsMod, MaxSwingsHardCap);
}

// ShouldUseWeaponHitCount mirrors weapon req level <= attacker level.
inline auto ShouldUseWeaponHitCount(const uint8 weaponReqLevel, const uint8 attackerLevel) -> bool
{
    return weaponReqLevel <= attackerLevel;
}

// ShouldApplyMaxSwingsMod mirrors isPC && MAX_SWINGS mod nonzero.
inline auto ShouldApplyMaxSwingsMod(const bool isPC, const uint8 maxSwingsMod) -> bool
{
    return isPC && maxSwingsMod != 0;
}

// ShouldApplyMobMultiHit mirrors multiHitMax > 0 for mob entities.
inline auto ShouldApplyMobMultiHit(const uint8 multiHitMax) -> bool
{
    return multiHitMax > 0;
}

// MobMultiHitSwingCount mirrors 1 + getHitCount(multiHitMax) host result.
// Host injects the rolled hit count from battleutils::getHitCount.
inline auto MobMultiHitSwingCount(const uint8 rolledHitCount) -> uint8
{
    return static_cast<uint8>(1 + rolledHitCount);
}

// ClampAttackRate mirrors std::clamp(rate, 0, 100) for DA/TA/QA.
inline auto ClampAttackRate(const int16 rate) -> int16
{
    if (rate < 0)
    {
        return 0;
    }
    if (rate > 100)
    {
        return 100;
    }
    return rate;
}

// IsMainHandForMultiHit mirrors (IsH2H && !swingsEmpty) || direction == RIGHTATTACK.
// Host injects isH2H, swingsEmpty, isRightAttack.
inline auto IsMainHandForMultiHit(const bool isH2H, const bool swingsEmpty, const bool isRightAttack) -> bool
{
    return (isH2H && !swingsEmpty) || isRightAttack;
}

// ShouldAddMikageSwings mirrors Mikage status && weapon is main-hand weapon.
inline auto ShouldAddMikageSwings(const bool hasMikage, const bool weaponIsMainHandWeapon) -> bool
{
    return hasMikage && weaponIsMainHandWeapon;
}

// Multi-hit preference outcome for the exclusive DA/TA/QA/mythic path.
// Host rolls 0-99 for each chance check; preference is evaluated top-down.
enum class MultiHitPreference : uint8
{
    None            = 0,
    Quad            = 1,
    Triple          = 2,
    Double          = 3,
    MythicThrice    = 4, // +2 swings NORMAL
    MythicTwice     = 5, // +1 swing NORMAL
    OccasionalExtra = 6, // num-1 NORMAL
};

// ResolveExclusiveMultiHitPreference mirrors Mikage-excluded exclusive ladder:
// Quad > Triple > Double > Mythic thrice > Mythic twice > occasional num>1.
// rolled values are 0..99 style (true if rolled < rate).
inline auto ResolveExclusiveMultiHitPreference(
    const bool quadProcs,
    const bool tripleProcs,
    const bool doubleProcs,
    const bool isMainHand,
    const bool mythicThriceProcs,
    const bool mythicTwiceProcs,
    const uint8 weaponSwingCount) -> MultiHitPreference
{
    if (quadProcs)
    {
        return MultiHitPreference::Quad;
    }
    if (tripleProcs)
    {
        return MultiHitPreference::Triple;
    }
    if (doubleProcs)
    {
        return MultiHitPreference::Double;
    }
    if (isMainHand && mythicThriceProcs)
    {
        return MultiHitPreference::MythicThrice;
    }
    if (isMainHand && mythicTwiceProcs)
    {
        return MultiHitPreference::MythicTwice;
    }
    if (weaponSwingCount > 1)
    {
        return MultiHitPreference::OccasionalExtra;
    }
    return MultiHitPreference::None;
}

// ExclusiveMultiHitSwingCount returns swings to AddAttackSwing for preference
// (excluding Mikage which uses shadow count). OccasionalExtra is num-1.
inline auto ExclusiveMultiHitSwingCount(const MultiHitPreference pref, const uint8 weaponSwingCount) -> uint8
{
    switch (pref)
    {
        case MultiHitPreference::Quad:
            return 4;
        case MultiHitPreference::Triple:
            return 3;
        case MultiHitPreference::Double:
            return 2;
        case MultiHitPreference::MythicThrice:
            return 2;
        case MultiHitPreference::MythicTwice:
            return 1;
        case MultiHitPreference::OccasionalExtra:
            return static_cast<uint8>(weaponSwingCount - 1);
        default:
            return 0;
    }
}

// MultiHitOccurred mirrors multiHitOccurred for QA/TA/DA only (not mythic/occasional).
inline auto MultiHitOccurred(const MultiHitPreference pref) -> bool
{
    return pref == MultiHitPreference::Quad || pref == MultiHitPreference::Triple || pref == MultiHitPreference::Double;
}

// ShouldAddDefaultHit mirrors multiHitOccurred == false.
inline auto ShouldAddDefaultHit(const bool multiHitOccurred) -> bool
{
    return !multiHitOccurred;
}

// ShouldAddAdditionalSwing mirrors isPC && rolled < ADDITIONAL_SWING_CHANCE.
inline auto ShouldAddAdditionalSwing(const bool isPC, const bool additionalSwingProcs) -> bool
{
    return isPC && additionalSwingProcs;
}

// ShouldAddOffhandExtraDualWield mirrors !isMainHand for EXTRA_DUAL_WIELD_ATTACK.
inline auto ShouldAddOffhandExtraDualWield(const bool isMainHand) -> bool
{
    return !isMainHand;
}

// AmbushRotationWindow mirrors abs(rotation diff) < 23 for Ambush augment.
inline auto AmbushRotationInWindow(const int16 rotationDiffAbs) -> bool
{
    return rotationDiffAbs < 23;
}

// ShouldApplyAmbushTripleBonus mirrors hasTrait Ambush && AUGMENTS_AMBUSH > 0 && rotation window.
inline auto ShouldApplyAmbushTripleBonus(
    const bool hasAmbushTrait,
    const bool hasAugmentsAmbushMod,
    const bool rotationInWindow) -> bool
{
    return hasAmbushTrait && hasAugmentsAmbushMod && rotationInWindow;
}

} // namespace attackroundhelpers
